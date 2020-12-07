CC = g++
CFLAGS = -std=c++2a -o
LDAP = -lldap -llber
PTHREAD = -lpthread

all: server client

server: src/server.cpp
	$(CC) $(CFLAGS) ./server src/server.cpp -I include $(LDAP) $(PTHREAD)

client: src/client.cpp
	$(CC) $(CFLAGS) ./client src/client.cpp -I include

clean:
	rm -f ./client ./server