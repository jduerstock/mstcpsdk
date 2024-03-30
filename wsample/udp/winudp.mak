all: winudp.exe

winudp.res: winudp.rc winudp.h
    rc -r winudp.rc

winudp.obj: winudp.c winudp.h
    cl -c -AS -Gsw -Oas -Zpei winudp.c

# This assumes that win_sock.lib is in a location specified in your LIB 
# environment varible. 
# Note also that win_sock.dll must be somewhere in you path, to run winupd.exe.
winudp.exe: winudp.obj winudp.def 
    link /MAP /NOD /CO winudp, , , slibcew libw wsockets, winudp.def
    rc winudp.res

winudp.exe: winudp.res
    rc winudp.res
