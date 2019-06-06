CC=clang
CFLAGS=-O2 -Wall -std=c11


all: first next

first: main.c lmem.c lmem.h
	$(CC) $(CFLAGS) main.c lmem.c -o first

next: main.c lmem.c lmem.h
	$(CC) $(CFLAGS) main.c lmem.c -o next -DNEXT_FIT

clean:
	rm first next
