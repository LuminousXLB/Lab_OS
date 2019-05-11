CC=clang
CFLAGS=-Wall -O2

CSAPP=csapp.o errutils.o rio.o

all: server client

server: server.o $(CSAPP) servant
	$(CC) server.o $(CSAPP) -o server -lpthread

client: client.o $(CSAPP)
	$(CC) client.o $(CSAPP) -o client -lpthread

servant: servant.o $(CSAPP)
	$(CC) servant.o $(CSAPP) -o servant -lpthread

clean:
	rm *.o