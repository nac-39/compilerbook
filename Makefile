CFLAGS=-std=c11 -g -static
SRCS=$(wildcard ./src/*.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
		$(CC) -o ./bin/9cc $(OBJS) $(LDFLAGS)

$(OBJS): ./src/9cc.h

test: ./bin/9cc
		./test.sh

clean:
		rm -f ./bin/9cc *.o *~ tmp*

.PHONY: test clean