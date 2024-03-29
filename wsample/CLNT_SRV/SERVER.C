/****************************************************************************

    PROGRAM: server.c

    PURPOSE: server sample application
       This is a very simple application to demonstate a stream connection 
       over the windows sockets dll.  It listens at a hard coded port 1041.
       It then receives packets and sends back a reply, until it receives 
       a packet telling it to close the connection.


    FUNCTIONS:
      called by windows ----
	WinMain() - calls initialization function, processes message loop
	InitApplication() - initializes window data and registers window
	InitInstance() - saves instance handle and creates main window
	MainWndProc() - processes messages
	AboutProc() - processes messages for "About" dialog box

      internal procedures ----
        srv_setup() - Sets the timer, loads the windows sockets dll, 
                      Does the listen.
        srv_connect() - Does the accept of the connection when its is ready
                        to complete.
        data_xfer() - Does a receive followed by a send each time it is called.
                      When the CLOSE message is received, calls cleanup();
        cleanup() - Kills the timer, closes the socket and frees the windows 
                    socket dll.
        inc_nI() - Increments the line number in the screen output buffer.

****************************************************************************/

#include "windows.h"		   
#include "server.h"		  

/* you must define WSOCKETS_DLL before including socket.h */
#define  WSOCKETS_DLL
#include <sys\socket.h>
#include <netinet\in.h>
#include <netdb.h>

/* Variables used in windows calls */
HANDLE 		hInst;		    /* current instance			     */
short		xChar, yChar ;

/* variables for the screen output buffer */
char szTextarr[MAXLINES][80];
short  nTextlarr[MAXLINES];
int  nI;

/* Application state variable.   Possible values are: NOT_STARTED, 
* WAITING_CONNECTION, and XFER_ACTIVE. 
*/
int    nTest_state = NOT_STARTED; 

/* variables for sockets activity */
int    nListen_sd, nAcc_sd;
int    nTotal_packets;
char   sRecvbuff[BUFFER_SIZE];
    
/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

    COMMENTS:

        Windows recognIzes this function by name as the initial entry point 
        for the program.  This function calls the application initialization 
        routine, if no other instance of the program is running, and always 
        calls the instance initialization routine.  It then executes a message 
        retrieval and dispatch loop that is the top-level control structure 
        for the remainder of execution.  The loop is terminated when a WM_QUIT 
        message is received, at which time this function exits the application 
        instance by returning the value passed by PostQuitMessage(). 

        If this function must abort before entering the message loop, it 
        returns the conventional value NULL.  

****************************************************************************/

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;			     /* current instance	     */
HANDLE hPrevInstance;			     /* previous instance	     */
LPSTR lpCmdLine;			     /* command line		     */
int nCmdShow;				     /* show-window type (open/icon) */
{
    MSG msg;				     /* message			     */

    if (!hPrevInstance)			 /* Other instances of app running? */
	if (!InitApplication(hInstance)) /* Initialize shared things */
	    return (FALSE);		 /* Exits if unable to initialize     */

    /* Perform initializations that apply to a specific instance */

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    /* Acquire and dispatch messages until a WM_QUIT message is received. */

    while (GetMessage(&msg,	   /* message structure			     */
	    NULL,		   /* handle of window receiving the message */
	    NULL,		   /* lowest message to examine		     */
	    NULL))		   /* highest message to examine	     */
	{
	TranslateMessage(&msg);	   /* Translates virtual key codes	     */
	DispatchMessage(&msg);	   /* Dispatches message to window	     */
    }
    
    return (msg.wParam);	   /* Returns the value from PostQuitMessage */
}


/****************************************************************************

    FUNCTION: InitApplication(HANDLE)

    PURPOSE: Initializes window data and registers window class

    COMMENTS:

        This function is called at initialization time only if no other 
        instances of the application are running.  This function performs 
        initialization tasks that can be done once for any number of running 
        instances.  

        In this case, we initialize a window class by filling out a data 
        structure of type WNDCLASS and calling the Windows RegisterClass() 
        function.  Since all instances of this application use the same window 
        class, we only need to do this when the first instance is initialized.  


****************************************************************************/

BOOL InitApplication(hInstance)
HANDLE hInstance;			       /* current instance	     */
{
    HANDLE hMemory;
    PWNDCLASS  pWndClass;
    BOOL bSuccess;

    hMemory = LocalAlloc((LMEM_FIXED | LMEM_ZEROINIT),sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);

    /* Fill in window class structure with parameters that describe the       */
    /* main window.                                                           */
    
    pWndClass->style = NULL;             /* Class style(s).                   */
    pWndClass->lpfnWndProc = MainWndProc;    /* Function to retrieve messages */
                                         /* windows of this class.            */
    pWndClass->cbClsExtra = 0;           /* No per-class extra data.          */
    pWndClass->cbWndExtra = 0;           /* No per-window extra data          */
    pWndClass->hInstance = hInstance;    /* Application that owns the class.  */
    pWndClass->hIcon     = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->hCursor   = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH); 
    pWndClass->lpszMenuName =  "ServerMenu";   /* Name of menu resounRce in .RC file. */
    pWndClass->lpszClassName = "ServerWClass";   /* Name used in call to CreateWindow. */

    /* Register the window class and return success/failure code. */

    bSuccess= (RegisterClass(pWndClass));

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bSuccess);

}


/****************************************************************************

    FUNCTION:  InitInstance(HANDLE, int)

    PURPOSE:  Saves instance handle and creates main window

    COMMENTS:

        This function is called at initialization time for every instance of 
        this application.  This function performs initialization tasks that 
        cannot be shared by multiple instances.  

        In this case, we save the instance handle in a static variable and 
        create and display the main program window.  
        
****************************************************************************/

BOOL InitInstance(hInstance, nCmdShow)
    HANDLE          hInstance;          /* Current instance identifier.       */
    int             nCmdShow;           /* Param for first ShowWindow() call. */
{
    HWND            hWnd;               /* Main window handle.                */

    /* Save the instance handle in static variable, which will be used in  */
    /* many subsequence calls from this application to Windows.            */
	
    hInst = hInstance;

    /* Create a main window for this application instance.  */

    hWnd = CreateWindow(
        "ServerWClass",                  /* See RegisterClass() call.          */
        "Server Sample Application",    /* Text for window title bar.       */
        WS_OVERLAPPEDWINDOW,            /* Window style.                      */
        CW_USEDEFAULT,                  /* Default horizontal position.       */
        CW_USEDEFAULT,                  /* Default vertical position.         */
        CW_USEDEFAULT,                  /* Default width.                     */
        CW_USEDEFAULT,                  /* Default height.                    */
        NULL,                           /* Overlapped windows have no parent. */
        NULL,                           /* Use the window class menu.         */
        hInstance,                      /* This instance owns this window.    */
        NULL                            /* Pointer not needed.                */
    );

    /* If window could not be created, return "failure" */

    if (!hWnd)
        return (FALSE);

    /* Make the window visible; update its client area; and return "success" */

    ShowWindow(hWnd, nCmdShow);  /* Show the window                        */
    UpdateWindow(hWnd);          /* Sends WM_PAINT message                 */
    return (TRUE);               /* Returns the value from PostQuitMessage */

}

/****************************************************************************

    FUNCTION: MainWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_CREATE     - create window 
	WM_COMMAND messages
            IDM_SRVSTART - start connection
            IDM_ABOUT - display About dialog box 
	WM_TIMER      - set timer interval has expired
	WM_DESTROY    - destroy window

****************************************************************************/

long FAR PASCAL MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;				  /* window handle		     */
unsigned message;			  /* type of message		     */
WORD wParam;				  /* additional information	     */
LONG lParam;				  /* additional information	     */
{
    FARPROC 	lpProc;		          /* pointer to a function           */
    HDC	    	hDC;
    TEXTMETRIC	tm;
    PAINTSTRUCT	ps;

    int nJ;

    switch (message) {
	
        case WM_CREATE:
	    hDC = GetDC(hWnd);
	    GetTextMetrics(hDC, &tm);
	    xChar = tm.tmAveCharWidth;
	    yChar = tm.tmHeight + tm.tmExternalLeading;
	    ReleaseDC(hWnd, hDC);
	    break;
	    
        case WM_PAINT:
            /* This program has a simplistic output method.  It repaints
            * the entire screen for every WM_PAINT message.  Each time
            * it adds a line of output it calls inc_nI() to increment the
            * line number in the output buffer, and it calls InvalidateRect()
            * so the screen will be repainted.
            */
            if (nI==0) {
                return (DefWindowProc(hWnd, message, wParam, lParam));
            }
            BeginPaint(hWnd,&ps);
            for (nJ=1;nJ<=nI;nJ++) {
                TextOut(ps.hdc,xChar,yChar *nJ,&szTextarr[nJ-1][0],nTextlarr[nJ-1]);
            }
            EndPaint (hWnd, &ps);
            break;

	case WM_COMMAND:	   /* message: command from application menu */
            switch (wParam){
                case IDM_SRVSTART:
	            if ((nTest_state == XFER_ACTIVE) || (nTest_state==WAITING_CONNECTION)) {
	                MessageBox(hWnd, 
		        "Already executing sample server test. PLEASE wait.",
		        "Error",
		        MB_OK);
	             } else {
			 srv_setup(hWnd);
                     }
		    break;
		    
                case IDM_ABOUT:
                    lpProc = MakeProcInstance(AboutProc, hInst);
                    DialogBox(hInst, "AboutBox", hWnd, lpProc);
                    FreeProcInstance(lpProc);
                    break;
            }
            break;

	case WM_TIMER:		         /* message: settime expired */
            if (nTest_state==XFER_ACTIVE){
                 data_xfer(hWnd); 
            }
            else {
               if (nTest_state==WAITING_CONNECTION) {
                   srv_connect(hWnd);
               }
               else {
	             MessageBox(hWnd, 
		     "state error, timer expired and not in XFER_ACTIVE or WAITING_CONNECTION state.",
		     "Error",
		      MB_OK);
	       } 
            }
	    break;
	case WM_DESTROY:		  /* message: window being destroyed */
	    if ((nTest_state == XFER_ACTIVE) || (nTest_state==WAITING_CONNECTION)) 
                cleanup(hWnd);
	    PostQuitMessage(0);
	    break;

	default:			  /* Passes it on if unproccessed    */
	    return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}

/**************************************************************************

    FUNCTION: srv_setup(HWND)

    PURPOSE: Set timer, load win_sock.dll, do a listen and set the 
    application state to WAITING_CONNECTION. 

**************************************************************************/

VOID PASCAL srv_setup(hWnd)
HWND  hWnd;

{
    int nRc;
    struct sockaddr_in server_addr;

    /* Set a timer to get control back at set intervals. */
    if (SetTimer(hWnd,1,TIMER_INTERVAL,NULL) == 0){
        MessageBox(hWnd, 
        "Help! Can't set timer.",
        "Error",
        MB_OK);
        return;
    }

    /* initialized index in screen output buffer */
    nI=0;

    /*
     ** create listen socket
    */
    nListen_sd= socket( AF_INET, SOCK_STREAM, 0);
    if (nListen_sd < 0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"socket call failed, error=%d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd);
        return;
    }

    /*
    ** bind to wildcard IP address and hard coded port 
    */
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(SRVPORT);       
    server_addr.sin_addr.s_addr=INADDR_ANY;
    nRc=bind( nListen_sd, (struct sockaddr far *) &server_addr, 
                sizeof(server_addr));
    if (nRc <0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"bind call failed, error=%d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd);
        return;
    }

    /*
    **  Define listen queue with space for 1 connection
    */
    nRc=listen( nListen_sd, 1);
    if (nRc <0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"listen failed, error=%d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd);
        return;
    }
    nTest_state=WAITING_CONNECTION;
}

/**************************************************************************

    FUNCTION: srv_connect(HWND)

    PURPOSE: Accept the connection and set the application state to 
    XFER_ACTIVE.
**************************************************************************/

VOID PASCAL srv_connect(hWnd)
HWND hWnd;
{
    int nRc;
    int nfnds, isset_r;
    fd_set fdreads;
    struct timeval timeout;

    /* see if connection has completed */
    FD_ZERO((fd_set far *) &fdreads); 
    FD_SET(nListen_sd, (fd_set far *)&fdreads);
    nfnds=32; 
    timeout.tv_sec=0;
    timeout.tv_usec=0;

    nRc= select(nfnds,&fdreads,NULL,NULL,&timeout);
    if (nRc <0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"select call failed, error=%d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd);
        return;
    }

    isset_r=FD_ISSET(nListen_sd, (fd_set far *) &fdreads);
    if (isset_r == 0) {
       nTextlarr[nI]=sprintf(&szTextarr[nI][0],"connect not ready");
       inc_nI(hWnd);
       InvalidateRect(hWnd,NULL,TRUE);
       return;
    }
     
    /* if connection complete do the accept */
    nAcc_sd=accept( nListen_sd, NULL, NULL);
    if (nAcc_sd <0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"Accept call failed, error=%d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd);
        return;
    }
     
    nTest_state=XFER_ACTIVE;
}

/****************************************************************************

    FUNCTION: data_xfer(hWnd)

    PURPOSE: Do a send and a receive.  Check the first char of the buffer 
    to see when to quit.

****************************************************************************/
VOID PASCAL data_xfer(hWnd) 
HWND    hWnd;
{
    int nTotal,nLen,nRc;
    long lIoctlarg; 

    /* check to see if there is anything to receive */

    nRc = ioctl(nAcc_sd,FIONREAD,(char * far) &lIoctlarg); 
    if (nRc <0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"ioctl call failed, error=%d",errno);
        cleanup(hWnd);
        return;
    }
    if (lIoctlarg == 0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"Waiting to receive packet");
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
        return;
    }
    
    /* receive */

    nTotal =0; 
    do {
        nLen = recv( nAcc_sd, &sRecvbuff[nTotal], BUFFER_SIZE-nTotal,
                        0);
        if ( nLen < 0 ) {
           /* break out of do while loop */
           break;
        } 
        nTotal += nLen;
    } while ( nTotal < BUFFER_SIZE );

    if (nLen <0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"Recieve called failed, error=%d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd);
        return;
    } /* if*/

    if (sRecvbuff[0] ==CLOSE) {
        cleanup(hWnd);
        return;
    }

    /* Increment packet count. (Close packet is not counted.) */
    nTotal_packets++;

    /* send the same packet back */

    nLen = send(nAcc_sd,sRecvbuff,BUFFER_SIZE,0);
    if (nLen <0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"send call failed, error=%d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd);
        return;
    } /* if */

    nTextlarr[nI]=sprintf(&szTextarr[nI][0],"packet %d received", nTotal_packets);
    inc_nI();
    InvalidateRect(hWnd,NULL,TRUE);
}

/****************************************************************************

    FUNCTION: cleanup(HWND)

    PURPOSE:  Kills the timer, closes sockets, frees win_sock.dll and 
    reinitializes state variables.

****************************************************************************/

VOID PASCAL cleanup(hWnd)
HWND   hWnd;
{
    int nRc;

    KillTimer(hWnd,1);

    nTest_state=NOT_STARTED;

    /* close sockets */
    nRc= close_socket( nListen_sd);
    if (nRc <0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"close call failed, error= %d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
    }

    nRc= close_socket(
                        nAcc_sd,
                        );
    if (nRc <0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"closed call failed, error=%d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
    }

    nTextlarr[nI]=sprintf(&szTextarr[nI][0],"End of program. %d packets received.",nTotal_packets);
    inc_nI();
    InvalidateRect(hWnd,NULL,TRUE);

    nTotal_packets=0;
}

/****************************************************************************

    FUNCTION: About(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for "About" dialog box

    MESSAGES:

        WM_INITDIALOG - initialize dialog box
        WM_COMMAND    - Input received

****************************************************************************/

BOOL FAR PASCAL AboutProc(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    switch (message) {
        case WM_INITDIALOG:
            return (TRUE);

        case WM_COMMAND:
            if (wParam == IDOK
                || wParam == IDCANCEL) {
                EndDialog(hDlg, TRUE);
                return (TRUE);
            }
            return (TRUE);
    }
    return (FALSE);
}

/****************************************************************************

    FUNCTION: inc_nI

    PURPOSE: Increments the line number in the screen output buffer.

****************************************************************************/

VOID PASCAL inc_nI() 
{
        if (nI <MAXLINES -1) {
                nI++;
        }
        else {
                nI++;
                nI =0;
                nTextlarr[nI]=sprintf(&szTextarr[nI][0],"--wrapping around--");
                nI = 1;
        }
}

