all: pipe server client submitter instagrapd worker signals reader

signals: signal1.c signal2.c signal3.c
	gcc -o signal1 signal1.c
	gcc -o signal2 signal2.c
	gcc -o signal3 signal3.c

reader: reader.c
	gcc -o reader reader.c

pipe: pipe.c
	gcc -o pipe pipe.c

server: server.c
	gcc -g -o server server.c

client: client.c
	gcc -o client client.c

submitter: submitter.c
	gcc -o submitter submitter.c

instagrapd: instagrapd.c
	gcc -o instagrapd instagrapd.c

worker: worker.c
	gcc -o worker worker.c

clean:
	rm -rf pipe client server client submitter worker signal1 signal2 signal3 instagrapd reader
