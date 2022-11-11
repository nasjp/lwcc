CFLAGS=-std=c11 -g -static

lwcc: lwcc.c

test: lwcc
	bash test.sh

clean:
	rm -f lwcc *.o *~ tmp*

.PHONY: test clean
