/* Message box and dialog box identifers */
#define IDM_SRVSTART     110
#define IDM_ABOUT        111
#define IDPOK            1
#define IDCANCEL         2

/* hard coded port to listen at */
#define SRVPORT		 1041

/*  Connections states */
#define	NOT_STARTED 	 0
#define	WAITING_CONNECTION  1
#define	XFER_ACTIVE      2

/* total number of sends */
#define NUM_SENDS        10

/* data buffer size */
#define BUFFER_SIZE      1024

/* maximum number of lines of output */
#define MAXLINES         20

/* time value for SetTimer call in milliseconds */  
#define TIMER_INTERVAL 	 5000     

/* packet types */
#define DATA  2
#define CLOSE 3

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE); 
BOOL InitInstance(HANDLE, int); 
long FAR PASCAL MainWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL AboutProc(HWND, unsigned, WORD, LONG);

VOID PASCAL srv_setup(HWND);
VOID PASCAL srv_connect(HWND);
VOID PASCAL data_xfer(HWND);
VOID PASCAL cleanup(HWND);
VOID PASCAL inc_nI();
