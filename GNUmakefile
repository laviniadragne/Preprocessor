CC = gcc -g
CFLAGS = -Wall -Wextra

build:	tema1.o functions.o hashtable.o linkedlist.o
	$(CC) $(CFLAGS) -o so-cpp tema1.o functions.o hashtable.o linkedlist.o

tema1.o: tema1.c
	$(CC) $(CFLAGS) -c tema1.c

functions.o: functions.c
	$(CC) $(CFLAGS) -c functions.c

hashtable.o: hashtable.c
	$(CC) $(CFLAGS) -c hashtable.c

linkedlist.o: linkedlist.c
	$(CC) $(CFLAGS) -c linkedlist.c

clean:
	rm -f *.o so-cpp