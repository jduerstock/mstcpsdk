
/****************************************************************************

    PROGRAM: winudp.c

    PURPOSE: sample UDP application
       This is a very simple application to demonstate the use of the 
       windows sockets dll.  It creates 2 UDP sockets and does a send
       from one and a receive at the other.


    FUNCTIONS:
      called by windows ----
	WinMain() - calls initialization function, processes message loop
	InitApplication() - initializes window data and registers window
	InitInstance() - saves instance handle and creates main window
	MainWndProc() - processes messages
	AboutProc() - processes messages for "About" dialog box

      internal procedures ----
        udp_cmds() - Loads the windows sockets dll, creates 2 UDP 
                     sockets, does and send and receive and calls cleanup.
        cleanup() - Closes sockets and free the windows sockets dll.
        inc_nI() - Increments the line number in the screen output buffer.

****************************************************************************/

#include "windows.h"		   
#include "winudp.h"		  

/* you must define WSOCKETS_DLL before including socket.h */
#define  WSOCKETS_DLL
#include <sys\socket.h>
#include <netinet\in.h>
#include <netdb.h>

/* Variables used in windows calls */
HANDLE 		hInst;		    /* current instance			     */
short		xChar, yChar ;

/* variables for the screen output buffer */
char sztextarr[MAXLINES][80];
short  ntextlarr[MAXLINES];
int  nI;

/* send and receive buffers */
char ssendbuff[BUFFER_SIZE];
char srecvbuff[BUFFER_SIZE];

/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

    COMMENTS:

        Windows recognizes this function by name as the initial entry point 
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

    hMemory = LocalAlloc((LMEM_FIXED | LMEM_ZEROINIT), sizeof(WNDCLASS));
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
    pWndClass->lpszMenuName =  "UDPMenu";   /* Name of menu resource in .RC file. */
    pWndClass->lpszClassName = "UDPWClass";   /* Name used in call to CreateWindow. */

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
        "UDPWClass",                    /* See RegisterClass() call.          */
        "UDP Sample Application",       /* Text for window title bar.         */
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
    UpdateWindow(hWnd);          /* sends WM_PAINT message                 */
    return (TRUE);               /* Returns the value from PostQuitMessage */

}

/****************************************************************************

    FUNCTION: MainWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

        WM_CREATE     - create window
        WM_PAINT      - repaints the window
        WM_DESTROY    - destroy window

        WM_COMMAND messages:
            IDM_UDP   - do some UDP commands.

    COMMENTS:

****************************************************************************/

long FAR PASCAL MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC     lpProcAbout;
    HMENU       hMenu;
    HDC         hDC;
    PAINTSTRUCT ps;
    TEXTMETRIC  tm;

    int nJ;

    switch (message) {

        case WM_CREATE:
            hDC =GetDC(hWnd);
            GetTextMetrics (hDC,&tm);
            xChar = tm.tmAveCharWidth;
            yChar = tm.tmHeight +tm.tmExternalLeading;
            ReleaseDC (hWnd,hDC); 
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
                TextOut(ps.hdc,xChar,yChar * nJ,&sztextarr[nJ-1][0],ntextlarr[nJ-1]);
            }
            EndPaint (hWnd, &ps);
            break;

        case WM_COMMAND:
            switch (wParam) {
                case IDM_UDP:
                    nI=0;
                    udp_cmds(hWnd);
                    break;

                case IDM_ABOUT:
                    lpProcAbout = MakeProcInstance(About, hInst);
                    DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
                    FreeProcInstance(lpProcAbout);
                    break;
                }
                break;

        case WM_DESTROY:
            PostQuitMessage(NULL);
            break;

        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}

/****************************************************************************

    FUNCTION: udp_cmds(HWND)

    PURPOSE:  Creates 2 sockets, binds address and send and receives a 
    packet.  When complete, or if and error occurs cleanup is called.

****************************************************************************/

VOID PASCAL udp_cmds(hWnd)
HWND hWnd;

{
    int nJ;
    int nFromLen;
    int nSd1,nSd2; 
    int nRc, nLen;
    struct hostent far *hp;
    struct sockaddr_in nSd1_addr, nSd2_addr, from;
    char far * szIP;

    ntextlarr[nI]=sprintf(&sztextarr[nI][0],"Starting UDP sample application");
                    inc_nI();
    InvalidateRect(hWnd,NULL,TRUE);

    /* create nSd1 socket */
    nSd1= socket( AF_INET, SOCK_DGRAM, 0);
    if (nSd1 < 0) {
        ntextlarr[nI]=sprintf(&sztextarr[nI][0],"1st socket call failed, error=%d ",errno);
        inc_nI();
        InvalidateRect(hWnd,NULL,TRUE);
        return;
    }

    /* create nSd2 socket */
    nSd2= socket( AF_INET, SOCK_DGRAM, 0);
    if (nSd2 < 0) {
        ntextlarr[nI]=sprintf(&sztextarr[nI][0],"2nd socket call failed, error=%d",errno);
        inc_nI();
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd, nSd1, nSd2); 
        return;
    }

    /* get IP address of local node */
    hp = gethostbyname( "local_node");
    if (hp ==NULL) {
        ntextlarr[nI]=sprintf(&sztextarr[nI][0],"can't continue, 'local_node' not found in hosts table");
        inc_nI();
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd, nSd1, nSd2); 
        return;
    }

    /* bind both sockets to hard coded ports */
    nSd1_addr.sin_family=AF_INET;
    nSd1_addr.sin_port=htons(PORT1);       
    nSd1_addr.sin_addr.s_addr=
                  *( (unsigned long far *) hp->h_addr_list[0]);
    nRc=bind( nSd1, (struct sockaddr far *) &nSd1_addr,
                sizeof(nSd1_addr));
    if (nRc <0) {
        ntextlarr[nI]=sprintf(&sztextarr[nI][0],"bind of socket nSd1=%d failed, error=%d",nSd1,errno);
        inc_nI();
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd, nSd1, nSd2);
        return;
    }

    nSd2_addr.sin_family=AF_INET;
    nSd2_addr.sin_port=htons(PORT2);       
    nSd2_addr.sin_addr.s_addr =
                  *( (unsigned long far *) hp->h_addr_list[0]);
    nRc=bind( nSd2, (struct sockaddr far *) &nSd2_addr,
                sizeof(nSd2_addr));
    if (nRc <0) {
        ntextlarr[nI]=sprintf(&sztextarr[nI][0],"bind of socket nSd2=%d failed, error=%d",nSd1,errno);
        inc_nI();
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd, nSd1, nSd2); 
        return;
    }

    /* initialize buffer */
    for (nJ=0; nJ<BUFFER_SIZE; nJ++)
        ssendbuff[nJ] =nJ;

    /* send data from nSd1 to nSd2 */
    nLen=sendto( nSd1, ssendbuff, BUFFER_SIZE, 0, 
                  (struct sockaddr * far) &nSd2_addr, 
                  sizeof(struct sockaddr_in));
    if (nLen <0) {
        ntextlarr[nI]=sprintf(&sztextarr[nI][0],"Winsendto failed, error=%d",errno);
        inc_nI();
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd, nSd1, nSd2);
        return;
    }
    else {
        ntextlarr[nI]=sprintf(&sztextarr[nI][0],"sent packet");
        inc_nI();
        InvalidateRect(hWnd,NULL,TRUE);
    }

    /* receive at nSd2 from nSd1 */
    nFromLen=sizeof(from);
    nLen =recvfrom( nSd2, srecvbuff, BUFFER_SIZE, 0,
                      (struct sockaddr * far) &from, &nFromLen);
    if (nLen <0) {
        ntextlarr[nI]=sprintf(&sztextarr[nI][0],"Winrecvfrom failed, error=%d",errno);
        inc_nI();
        InvalidateRect(hWnd,NULL,TRUE);
        cleanup(hWnd, nSd1, nSd2); 
        return;
    }
    else {
        szIP = inet_ntoa(from.sin_addr);
        ntextlarr[nI]=sprintf(&sztextarr[nI][0],"received packet from IP addr=%s", szIP);
        inc_nI();
        InvalidateRect(hWnd,NULL,TRUE);
    }
    ntextlarr[nI]=sprintf(&sztextarr[nI][0],"End of UDP sample program.");
    inc_nI();
    InvalidateRect(hWnd,NULL,TRUE);
    cleanup(hWnd, nSd1, nSd2);
}

/****************************************************************************

    FUNCTION: cleanup(HWND)

    PURPOSE:  Closes sockets and free the windows sockets DLL 

****************************************************************************/

VOID PASCAL cleanup(hWnd, nSd1, nSd2)
HWND  hWnd;
int	nSd1, nSd2;

{
    int nRc; 
    short errno;

    nRc= close_socket( nSd1);
    if (nRc < 0) {
        ntextlarr[nI]=sprintf(&sztextarr[nI][0],"Error on socket close.  socket=%d, error=%d",nSd1,errno);
        inc_nI();
        InvalidateRect(hWnd,NULL,TRUE);
    }

    nRc= close_socket( nSd2);
    if (nRc < 0) {
        ntextlarr[nI]=sprintf(&sztextarr[nI][0],"Error on socket close.  socket=%d, error=%d",nSd2,errno);
        inc_nI();
        InvalidateRect(hWnd,NULL,TRUE);
    }

}

/****************************************************************************

    FUNCTION: About(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for "About" dialog box

    MESSAGES:

        WM_INITDIALOG - initialize dialog box
        WM_COMMAND    - Input received

****************************************************************************/

BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
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
                ntextlarr[nI]=sprintf(&sztextarr[nI][0],"--wrapping around--");
                nI = 1;
        }
}
