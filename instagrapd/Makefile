all: instagrapd reader

reader: reader.c
	gcc -o reader reader.c

instagrapd: instagrapd.c
	gcc -o instagrapd instagrapd.c

clean:
	rm -rf instagrapd reader
