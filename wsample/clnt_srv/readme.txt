Client and server programs are provided to demonstate the use of a
stream connection using the wsockets.dll. You must put the entry
"win_server_node", with the IP address of the machine you are running
the server program on, in the host file used by the client program.
The port number 1041 is hard coded in the programs.  Both programs
may be run on the same machine.  The server program must be started
before the client program.  

To start the server program click on Options (the only menu choice). 
Then click on Starting sample server application. You will see the
output "connect not ready" at 5 second intervals.  After you
start the client you will see the output "packet 1 sent" in the client
window, followed by "packet 1 received" in the server window.  This will
continue for 10 packets. Then the client side will display "End of program.
10 packets sent." and the server side will display "End of program. 10
packets received."  You may occasionally see the message "Waiting for 
packet" on either side.  

If you wish to modify these program, first modify the makefiles, server.mak
and client.mak, if necessary, and then remake with pvcs make:

   make -f server
   make -f client
