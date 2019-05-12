CC=clang
CFLAGS=-Wall -O2 -std=gnu99


CSAPP=w_error.o w_rio.o w_signal.o w_netutil.o w_socket.o w_procmg.o w_unixio.o
# CSAPP=csapp.o w_error.o w_rio.o w_signal.o w_netutil.o w_socket.o w_procmg.o

W_SOCKET_DEP=w_error.o w_netutil.o w_rio.o w_unixio.o
# W_PROCMG_DEP=w_error.o
# W_SIGNAL_DEP=w_error.o


SERVER_DEP=$(W_SOCKET_DEP) w_procmg.o w_signal.o w_socket.o
CLIENT_DEP=$(W_SOCKET_DEP) w_socket.o
SERVANT_DEP=$(W_SOCKET_DEP) w_socket.o


all: server client

server: server.o $(SERVER_DEP) servant
	$(CC) server.o $(SERVER_DEP) -o server -lpthread

client: client.o $(CLIENT_DEP)
	$(CC) client.o $(CLIENT_DEP) -o client -lpthread

servant: servant.o $(SERVANT_DEP)
	$(CC) servant.o $(SERVANT_DEP) -o servant -lpthread

clean:
	rm *.o servant server client
	