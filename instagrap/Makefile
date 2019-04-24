all: server client


server: server.c
	gcc -g -o server server.c

client: client.c
	gcc -o client client.c

clean:
	rm -rf pipe client server client submitter worker signal1 signal2 signal3
