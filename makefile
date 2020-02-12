CFLAGS=-std=c11 -g -static

9cc: 9cc.c

# 引数を指定して実行: make expr="1+1" run
run :9cc
	./9cc ${expr}

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean