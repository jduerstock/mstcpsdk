/****************************************************************************

    PROGRAM: client.c

    PURPOSE: client sample application
        This is a very simple application to demonstrate a stream connection 
        over the windows sockets dll.  It establishes a connection to 
        win_server_node (This name must be in your hosts file.),
        at port number 1041 and then sends and receives 10 packets. After 
        the 10 packets are sent, a close packet is sent and the socket is
        closed.

    FUNCTIONS:
      Called by windows---- 
	WinMain() - calls initialization function, processes message loop
	InitApplication() - initializes window data and registers window
	InitInstance() - saves instance handle and creates main window
	MainWndProc() - processes messages
	AboutProc() - processes messages for "About" dialog box

      Called internally----
        cl_setup() - Sets the timer, loads the windows sockets dll, 
                     establishes the connection and does the 1st send.
        data_xfer() - does a receive followed by a send each time it is called.
                      When 10 send and receive are complete, calls cleanup.
        cleanup() - Kills the timer, sends the close packet, closes the socket
                    and frees the windows socket dll.
        inc_nI() - increments the line number in the screen output buffer.

****************************************************************************/

#include "windows.h"		    /* required for all Windows applications */
#include "client.h"		    /* specific to this program		     */

/* WSOCKETS_DLL must be defined before including socket.h */
#define  WSOCKETS_DLL
#include <sys\socket.h>
#include <netinet\in.h>
#include <netdb.h>

/* Global Variables used in windows calls */
HANDLE 		hInst;		    /* current instance			     */
short		xChar, yChar ;

/* variables for the screen output buffer */
char szTextarr[MAXLINES][80];
short  nTextlarr[MAXLINES];
int  nI;

/* The application state variable. Possible values are NOT_STARTED and 
* XFER_ACTIVE. 
*/
int	     nTest_state = NOT_STARTED; 

/* variables for socket activity */
int          nSd;
unsigned     nTotal_packets;
char         sRecvbuff[BUFFER_SIZE];
char         sSendbuff[BUFFER_SIZE];

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
    pWndClass->lpszMenuName =  "ClientMenu";   /* Name of menu resource in .RC file. */
    pWndClass->lpszClassName = "ClientWClass";   /* Name used in call to CreateWindow. */

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
        "ClientWClass",                 /* See RegisterClass() call.          */
        "Sample Client Application",    /* Text for window title bar.         */
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
	WM_PAINT      - repaint window 
	WM_COMMAND messages 
            IDM_CLSTART - start the connection
            IDM_ABOUT - display About dialog box 
	WM_TIMER      - Set timer interval has expired
	WM_DESTROY    - destroy window

    COMMENTS:

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
                case IDM_CLSTART:
	            if (nTest_state == XFER_ACTIVE) {
	                MessageBox(hWnd, 
		        "Already executing sample client application. PLEASE wait.",
		        "Error",
		        MB_OK);
	            } else {
                        cl_setup(hWnd); 
		    }
		    break;
		    
                case IDM_ABOUT:
                    lpProc = MakeProcInstance(AboutProc, hInst);
                    DialogBox(hInst, "AboutBox", hWnd, lpProc);
                    FreeProcInstance(lpProc);
                    break;

            }
            break;

	    
	case WM_TIMER:		         /* message: Timer message */
            if (nTest_state==XFER_ACTIVE) {
                data_xfer(hWnd); 
            }
            else {
	         MessageBox(hWnd, 
		 "state error, timer expired and not in XFER_ACTIVE state.",
		 "Error",
		  MB_OK);
	    } 
	    break;
	case WM_DESTROY:		  /* message: window being destroyed */
            if (nTest_state==XFER_ACTIVE)
                cleanup(hWnd);
	    PostQuitMessage(0);
	    break;

	default:			  /* Passes it on if unproccessed    */
	    return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}

/**************************************************************************

    FUNCTION: cl_setup(HWND)

    PURPOSE:  Sets a timer, loads win_sock.dll, establishes a connection 
    to the server and sends the first buffer.

**************************************************************************/

VOID PASCAL cl_setup(hWnd)
HWND  hWnd;
{
    int    nRc, nLen, nK;
    struct hostent far *hp;
    struct sockaddr_in server_addr;

    /* Set timer to get control back at reqular intervals. */
    if (SetTimer(hWnd,1,TIMER_INTERVAL,NULL) == 0){
        MessageBox(hWnd, 
        "Help! Can't set timer.",
        "Error",
        MB_OK);
        return;
    }

    /* initialize the index to the screen output buffer */
    nI=0;

    /* create a socket */
    nSd= socket( AF_INET, SOCK_STREAM, 0);
    if (nSd < 0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"socket call failed, error=%d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd);
        return;
    }

    /*
    ** Get IP address from host file.
    */
    hp = gethostbyname("win_server_node");
    if (hp == NULL ) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"win_server_node entry not in hosts file");
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd);
        return;
    }

    /*
    ** Connect to remote socket at hard coded port.
    */
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(SRVPORT);       
    server_addr.sin_addr.s_addr=
              *( (unsigned long far *) hp->h_addr_list[0]);
    
    nRc=connect( nSd, (struct sockaddr far *) &server_addr,
                   sizeof(server_addr));
    if (nRc <0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"Connect failed, error=%d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd);
        return;
    }

    /* initialize the packet  */
    for (nK=0; nK<BUFFER_SIZE; nK++) 
        sSendbuff[nK]=nK;

    /* send first data packet */
    sSendbuff[0]=DATA; 
    nLen = send( nSd, sSendbuff, BUFFER_SIZE, 0);
    if (nLen <0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"error on first send,  error=%d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd);
        return;
    }
   
    nTotal_packets++; 
    nTextlarr[nI]=sprintf(&szTextarr[nI][0],"packet %d sent", nTotal_packets);
    inc_nI();
    InvalidateRect(hWnd,NULL,TRUE);

    nTest_state=XFER_ACTIVE;

}

/****************************************************************************

    FUNCTION: data_xfer(hWnd)

    PURPOSE: Does a receive and then a send.  After  a nTotal of 10 sends  
      and receives the cleanup routine is called.

****************************************************************************/
VOID PASCAL data_xfer(hWnd) 
HWND    hWnd;
{
    int nLen, nRc;
    int nTotal;
    int nK;
    long lIoctlarg; 

    /* check to see if there is anything to receive */
    nRc = ioctl(nSd,FIONREAD,(char * far) &lIoctlarg); 
    if (nRc <0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"ioctl error=%d",errno);
        cleanup(hWnd);
        return;
    }
    if (lIoctlarg == 0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"Waiting to receive packet.");
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
        return;
    }

    /* receive packet */
    nTotal =0; 
    do {
        nLen = recv( nSd, &sRecvbuff[nTotal], BUFFER_SIZE-nTotal,
                       0);
        if ( nLen < 0 ) {
            /* break out of do while loop */
            break;
        } 
        nTotal += nLen;
    } while ( nTotal < BUFFER_SIZE );

    if (nLen <0)  {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"Receive failed, error=%d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd);
        return;
    } /* if*/
    
    /*  Check to see if reached NUM_SENDS total packets. */
    if (nTotal_packets >= NUM_SENDS) {
        cleanup(hWnd);
        return;
    }

    /* Send the next packet. */
    nLen = send(nSd,sSendbuff,BUFFER_SIZE,0);
    if (nLen <0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"send failed, error=%d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd);
        return;
    } /* if */

    /*  increment packets count */
    nTotal_packets++; 
   
    nTextlarr[nI]=sprintf(&szTextarr[nI][0],"packet %d sent", nTotal_packets);
    inc_nI();
    InvalidateRect(hWnd,NULL,TRUE);
}
/****************************************************************************

    FUNCTION: cleanup(HWND)

    PURPOSE:  Kills the timer, closes sockets, frees win_sock.dll
    and reinitializes state variables.


****************************************************************************/

VOID PASCAL cleanup(hWnd)
HWND   hWnd;
{
    int nRc, nLen;

    KillTimer(hWnd,1);

    nTest_state=NOT_STARTED;

    /* Send close packet.
    * note have to send sSendbuff size packet because the server is designed
    * to receive BUFFER_SIZE bytes of data and then check if it is
    * a close packet.
    */
    sSendbuff[0]=CLOSE; 
  
    /* send close packet */ 
    nLen = send( nSd, sSendbuff, BUFFER_SIZE, 0);
    if (nLen <0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"send of close packet failed, error=%d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
    }

    nTextlarr[nI]=sprintf(&szTextarr[nI][0],"End of program. %d packets sent.",nTotal_packets);
    inc_nI();
    InvalidateRect(hWnd,NULL,TRUE);

    nRc= close_socket(
                        nSd,
                        );
    if (nRc <0) {
        nTextlarr[nI]=sprintf(&szTextarr[nI][0],"socket closed failed, error=%d",errno);
        inc_nI(hWnd);
        InvalidateRect(hWnd,NULL,TRUE);
    }
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
                /*InvalidateRect(hWnd,NULL,TRUE); */
                nI =0;
                nTextlarr[nI]=sprintf(&szTextarr[nI][0],"--wrapping around--");
                nI = 1;
        }
}


