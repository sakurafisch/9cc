CFLAGS=-std=c11 -g -static

main: main.c

test: main
		./test.sh

clean:
		rm -f main *.0 *~ tmp*

.PYONY: test clean
