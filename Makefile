CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

lwcc: $(OBJS)
	$(CC) -o lwcc $(OBJS) $(LDFLAGS)

$(OBJS):

test: lwcc
	./test.sh

clean:
	rm -f lwcc *.o *~ tmp*

.PHONY: test clean
