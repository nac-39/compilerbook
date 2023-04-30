CFLAGS=-std=c11 -g -static

9cc: ./src/9cc.c
		cc -o ./bin/9cc ./src/9cc.c $(CFLAGS)

test: ./bin/9cc
		./test.sh

clean:
		rm -f ./bin/9cc *.o *~ tmp*

.PHONY: test clean