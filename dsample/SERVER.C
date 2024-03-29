/*      COPYRIGHT, (c) HEWLETT PACKARD CO. 1990,1991 */
/*      All rights reserved. No part of this program */
/*      may be copied or used without the express    */
/*      written consent of HEWLETT PACKARD Corp.     */
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
            
#define BUFFER_SIZE 1000
            
char sendbuf[BUFFER_SIZE];
char recvbuf[BUFFER_SIZE];
            
struct sockaddr_in server_addr, client_addr;
            
int sleep( seconds )
int seconds;
{
    long start_seconds, current_seconds, end_seconds;
            
    time( &start_seconds );
    end_seconds = start_seconds + (long) seconds;
            
    do {
        time( &current_seconds );
    } while ( current_seconds < end_seconds );
    return( 0 );
}
            
            
/*
**  This short program will act as a network 'server' and will accept a
**  single incoming connection.  After the connection is established, data
**  will be received and then transmitted, and the server terminates
*/

int errno;


main( argc, argv )
int argc;
char *argv[];
{
    int rc, i, len, total, listen_sd, acc_sd;
    unsigned int size;
            
            
    if ( argc < 3 ) {
        printf( "Usage: %s <local_port> <xfer_size>\n", argv[0] );
        exit( 1 );
    }
            
    size = atoi( argv[2] );
            

    /*
    **  Create a socket locally to serve as a communication endpoint.
    */
            
    listen_sd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( listen_sd < 0 ) {
        printf( "Error: socket() call failed with errno=%d\n", errno );
        exit( 1 );
    }
    printf( "socket() returned %d\n", listen_sd );
            
            
    /*
    **  'Bind' the newly created socket to a wildcard local IP address and
    **  the local TCP port specified on the command line
    */
            
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons( atoi( argv[1] ) );
    server_addr.sin_addr.s_addr = INADDR_ANY;
            
    rc = bind( listen_sd, (struct sockaddr *) &server_addr,
                                           sizeof( server_addr ) );
    if ( rc < 0 ) {
        printf( "Error: bind() call failed w/errno=%d\n", errno );
        close_socket( listen_sd );
        exit( 1 );
    }
    printf( "bind() call returned %d\n", rc );
            
            
    /*
    **  Create a queue of length one to hold incoming connection requests.
    */
            
    printf( "server will listen on port %d\n", ntohs( server_addr.sin_port ) );
    rc = listen( listen_sd, 1 );
    if ( rc < 0 ) {
        printf( "Error: listen() call failed w/errno=%d\n", errno );
        close_socket( listen_sd );
        exit( 1 );
    }
    printf( "listen() call returned %d\n", rc );
            
            
    /*
    **  Call accept(), which will block until an incoming connection is
    **  established on the listening socket.  Accept() then returns the socket
    **  descriptor of the new socket and will also fill in the 'client_addr'
    **  socket address structure with the remote IP address and port.
    */
            
    len = sizeof( struct sockaddr_in );
    printf( "blocking on accept() waiting for an incoming connection...\n" );
    acc_sd = accept( listen_sd, (struct sockaddr *) &client_addr, &len );
    if ( acc_sd < 0 ) {
        printf( "Error: accept() call failed w/errno=%d\n", errno );
        close_socket( listen_sd );
        exit( 1 );
    }
    printf( "Established connection with 0x%lx\n", client_addr.sin_addr );
            
            
    /*
    **  The connection is now established.
    **
    **  We will receive all of the data sent by the remote node before sending
    **  data out. There is one caveat, however.  The recv() call is defined
    **  such that it returns as soon as ANY data is available rather than
    **  waiting until all 'size'  bytes of data havebeen read.  Therefore, we
    **  must loop on the recv() call until all data has been received.
    */
            
    total = 0;
    printf( "Now attempting to receive data...\n" );
    do {
        len = recv( acc_sd, &recvbuf[total], size-total, 0 );
        if ( len < 0 ) {
            printf( "Error: recv() call failed w/errno=%d\n", errno );
            close_socket( listen_sd );
            close_socket( acc_sd );
            exit( 1 );
        }
        printf( "recv() returned %d bytes\n", len );
        total += len;
    } while ( total < size );
            
            
    /*
    **  Echo the received data back to the remote node.
    */
            
    len = send( acc_sd, recvbuf, size, 0 );
    if ( len < 0 ) {
        printf( "Error: recv() call failed w/errno=%d\n", errno );
        close_socket( listen_sd );
        close_socket( acc_sd );
        exit( 1 );
    }
            
            
    /*
    **  All data has been received and transmitted, so close the sockets.
    */
            
    close_socket( acc_sd );
    close_socket( listen_sd );
    exit( 0 );
}

