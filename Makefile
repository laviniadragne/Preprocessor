CC = cl.exe
CFLAGS = /nologo /W3 /MD /D_CRT_SECURE_NO_DEPRECATE 

build:	tema1.obj functions.obj hashtable.obj linkedlist.obj
	$(CC) $(CFLAGS) /Feso-cpp.exe tema1.obj functions.obj hashtable.obj linkedlist.obj

tema1.obj:	tema1.c
	$(CC) $(CFLAGS) /Fotema1.obj /c tema1.c

functions.obj:	functions.c
	$(CC) $(CFLAGS) /Fofunctions.obj /c functions.c

hashtable.obj:	hashtable.c hashtable.h
	$(CC) $(CFLAGS) /Fohashtable.obj /c hashtable.c

linkedlist.obj:	linkedlist.c linkedlist.h
	$(CC) $(CFLAGS) /Folinkedlist.obj /c linkedlist.c

clean:
	del *.obj *.exe
