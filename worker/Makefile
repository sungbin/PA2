all: worker applier

applier: applier.c
	gcc -o applier applier.c

worker: worker.c
	gcc -o worker -pthread worker.c

clean:
	rm -rf worker applier *.out *.in
