default: all

all: clean compile test
.PHONY: all

compile: *.h *.c
	mkdir -p bin
	gcc --std=c99 -o bin/todo todotxt.c tasks.c app.c

test: *.h *.c
	mkdir -p bin
	gcc --std=c99 -o bin/test_todo todotxt.c tasks.c test_todo.c
	./bin/test_todo

clean:
	rm -rf bin/
.PHONY: clean

