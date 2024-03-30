/*      COPYRIGHT, (c) HEWLETT PACKARD CO. 1990,1991 */
/*      All rights reserved. No part of this program */
/*      may be copied or used without the express    */
/*      written consent of HEWLETT PACKARD Corp.     */
/*
**      WSOCKDEF.H
**
**      Copyright (c) Hewlett Packard Company, 1989.  All rights reserved.
**      No part of this program may be copied or used without the prior
**      written consent of Hewlett Packard Company.
**
*/

/*
**      System calls
*/

short far pascal WinAccept( unsigned int, short, struct sockaddr far *,
                                         short far *, short far * );
short far pascal WinBind( unsigned int, short, struct sockaddr far *,
                                         short, short far * );
short far pascal WinCloseSocket( unsigned int, short, short far * );
short far pascal WinConnect( unsigned int, short, struct sockaddr far *,
                                         short , short far * );
short far pascal WinGetHostname( char far *, short , short far * );
short far pascal WinGetPeerName( unsigned int, short, struct sockaddr far *,
                                         short far * , short far * );
short far pascal WinGetSockName( unsigned int, short, struct sockaddr far *,
                                         short far * , short far * );
short far pascal WinGetSockOpt( unsigned int, short, short, short, char far *,
                                         short far * , short far * );
short far pascal WinIoctl( unsigned int, short, short, char far * , short far * );
short far pascal WinListen( unsigned int, short, short , short far * );
short far pascal WinRecv( unsigned int, short, char far *, short, short,
                                                        short far * );
short far pascal WinRecvFrom( unsigned int, short, char far *, short, short,
                       struct sockaddr far *, short far * , short far * );
short far pascal WinSelect( unsigned int, short, fd_set far *, fd_set far *,
                             fd_set far *, struct timeval far * , short far * );
short far pascal WinSend( unsigned int, short, char far *, short, short,
                                                            short far * );
short far pascal WinSendTo( unsigned int, short, char far *, short, short,
                       struct sockaddr far *, short, short far * );
short far pascal WinSetSockOpt( unsigned int, short, short, short,
                       char far *, short, short far * );
short far pascal WinSocket( unsigned int, short, short, short , short far * );


/*
**  short far pascal  win_sock_strerror( short, char far * );
*/

short far pascal WinSockStrError( short, char far *);


/*
**    inet_*, byte swapping routines
*/

unsigned long   far pascal WinInetAddr( char far * );
long            far pascal WinInetLNAOF( struct in_addr );
struct in_addr  far pascal WinInetMakeAddr( unsigned long, unsigned long );
long            far pascal WinInetNetOF( struct in_addr );
unsigned long   far pascal WinInetNetwork( char far * );
short           far pascal WinInetNToA( struct in_addr, char far * );

unsigned short  far pascal WinHToNS( unsigned short );
unsigned short  far pascal WinNToHS( unsigned short );
unsigned long   far pascal WinHToNL( unsigned long );
unsigned long   far pascal WinNToHL( unsigned long );


/*
**    library calls
*/

short far pascal WinGetAsciiDBPath( char far * );

short far pascal WinGetHostByName( char far *, char far * );
short far pascal WinGetHostByAddr( struct in_addr far *, short,
                                               short, char far * );

short far pascal WinGetNetByName( char far *, char far * );
short far pascal WinGetNetByAddr( unsigned long, short, char far * );

short far pascal WinGetProtoByName( char far *, char far * );
short far pascal WinGetProtoByNumber( short, char far * );

short far pascal WinGetServByName( char far *, char far *, char far * );
short far pascal WinGetServByPort( short, char far *, char far * );


/*
**    Library initialization, free calls (for static lib interface)
*/

unsigned int far pascal WinInitSockLib( void );
void far pascal WinFreeSockLib( unsigned int );

