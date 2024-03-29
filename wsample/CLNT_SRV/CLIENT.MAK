all: client.exe

client.res: client.rc client.h
    rc -r client.rc

client.obj: client.c client.h
    cl -c -AS -Gsw -Oas -Zpe client.c

# win_sock.lib isn't in the dependency list to avoid having to specify a path.  
# Also assumes the path of wsockets.lib is in the lib environment variable.
client.exe: client.obj client.def 
    link /MAP /NOD client, , , slibcew libw wsockets, client.def
    rc client.res

# if the .res is new and the .exe is not, update the resource.     
# note that the .rc file can be updated without having to either compile
# or link the file
client.exe: client.res
    rc client.res
