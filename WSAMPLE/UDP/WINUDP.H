
#define IDM_UDP     101
#define IDM_ABOUT   104

#define PORT1       1025
#define PORT2       1026

#define BUFFER_SIZE 100

#define MAXLINES    20

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE); 
BOOL InitInstance(HANDLE, int); 
long FAR PASCAL MainWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);

VOID PASCAL inc_nI();
VOID PASCAL udp_cmds(HWND);
VOID PASCAL cleanup(HWND, int, int);
