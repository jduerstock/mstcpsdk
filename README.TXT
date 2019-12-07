	       Microsoft TCP/IP Sockets Development Kit

			     Version 1.0

		       SUPPLEMENTAL INFORMATION


Copyright (c) Hewlett-Packard Company, 1990.
Copyright (c) Microsoft, 1992. All rights reserved.

Microsoft, MS, and MS-DOS are registered trademarks and Windows is a
trademark of Microsoft Corporation. 

Operating System/2 and OS/2 are registered trademarks licensed to
Microsoft Corporation. U.S. Patent Number 4955066.



This file reflects last-minute information regarding the Microsoft
TCP/IP Sockets Development Kit version 1.0.

Installing the Microsoft TCP/IP Sockets Development Kit
-------------------------------------------------------

The following files are included in the Microsoft TCP/IP Sockets
Development Kit:

README.TXT  - this file

\INCLUDE\ - the same include files are used for MS-DOS/Windows and
	    OS/2 application development. By default, the libraries
	    are used for MS-DOS or OS/2 application development. To
	    designate Windows development, you must define the symbol
	    WSOCKETS_DLL before including these files.  See Windows
	    sample apps.

    NETDB.H - defines data structures used by the sockets network
    	      database routines (gethostent, getservent, etc.).

    NETINET\
       IN.H - definitions particular to the Internet address domain.

    SOCK_ERR.H - defines errors returned by sockets routines.

    SOCKDEFS.H - function prototypes for MS-DOS and OS/2 sockets
    	         routines.  (Note: included in socket.h).

    WSOCKDEF.H - function prototypes for Windows sockets routines.
    		 (Note: included in socket.h if WSOCKETS_DLL is
		 defined.) 
    SYS\
       SOCKET.H - contains socket definitions and type declarations 
                  necessary for most sockets applications.

\LIB\
    DOS_SOCK.LIB -  This library is for MS-DOS sockets applications.  

    OS2_SOCK.LIB -  This library is for OS/2 sockets applications.  

    WSOCKETS.LIB - This is a small static library that is used to
    		   handle the explicit loading of wsockets.dll.
		   wsockets.dll is included with the LAN Manager
		   product. See the following sections on Sockets
		   DLL/Library and Static vs. Dynamic Link Library
		   Interfaces.

\DSAMPLE\   - this directory contains sample code for an MS-DOS or
	      OS/2 (not Windows) application.

    CLIENT.C - sample client application source code.

    SERVER.C - sample server application source code.

WSAMPLE\ - this directory contains Windows sample code

    CLNT_SRV\  - sample client/server application source code for
    		 Windows. 

    UDP\  - source for a very simple UDP Windows application.

\NETPROG\

    SOCKETS.EXE - a patched version of socktsr.exe/sockets.exe that
    contains a bug fix not found in earlier versions of LAN Manager 2.1
    and Microsoft TCP/IP Utilities for LAN Manager.

You may freely copy the contents of this diskette to your hard disk,
taking care to preserve the include file directory structure.  After
indicating to your compiler and linker the location of the include
files and sockets libraries, you should be able to completely build
your application.


Sockets DLL/library
-------------------
The sockets DLL named 'wsockets.dll will be made available to end
users on the LAN Manager distribution diskettes. The user will be
required to copy this file manually from the distribution disk - it
is *not* copied during the installation of LAN Manager by default. It
is also important to remind users that they must load either
socktsr.exe (the old real-mode sockets TSR) or sockets.exe. 


Wsockets.DLL/sockets.exe bug fixes
----------------------------------
A bug fix was made to both wsockets.dll that caused some applications
to UAE when the remote system closed the socket first in a TCP
connection. This bug fix will be incorporated into all new releases
of Microsoft LAN Manager and The Microsoft TCP/IP Utilities for LAN
Manager. For your reference, these updates are provided on this
diskette. 


Memory Implications
-------------------
This implementation of sockets attempts to minimize the memory required
to execute sockets applications.  There are numerous situations in which
not enough resources are available to fully execute a sockets call, and
sockets indicates this to the user by returning the ENOBUFS error.  If
your application receives this error frequently, you should evaluate the
cause, and adjust your PROTOCOL.INI parameters to provide increased
resources.


Maximum SOCK_DGRAM send size
----------------------------
Currently, the maximum SOCK_DGRAM (UDP) send size is limited to 1400
bytes by the networking services below sockets.  IP fragmentation on
UDP transmission is likewise not yet supported.
