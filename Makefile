CC=gcc 
CFLAGS=-Wall -g


build: tema1.c
	$(CC) $(CFLAGS) main.c -o quadtree

run: build
	./tema1

clean:
	rm -f *.o exec

archive:
	zip -r tema2.zip Makefile README *.c