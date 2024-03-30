all: server.exe

server.res: server.rc server.h
    rc -r server.rc

server.obj: server.c server.h
    cl -c -AS -Gsw -Oas -Zpe server.c

# win_sock.lib isn't in the dependency list to avoid having to specify a path.  
# Also assumes the path of wsockets.lib is in the lib environment variable.
server.exe: server.obj server.def 
    link /MAP /NOD server, , , slibcew libw wsockets, server.def
    rc server.res

# if the .res is new and the .exe is not, update the resource.     
# note that the .rc file can be updated without having to either compile
# or link the file
server.exe: server.res
    rc server.res
