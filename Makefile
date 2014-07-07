default: all

all: clean compile test
.PHONY: all

compile: src/*.h src/*.c
	mkdir -p build
	gcc --std=c99 -o build/todo src/todotxt.c src/tasks.c src/app.c

test: src/*.h src/*.c
	mkdir -p build
	gcc --std=c99 -o build/test_todo src/todotxt.c src/tasks.c src/test_todo.c
	./build/test_todo

clean:
	rm -rf build/
.PHONY: clean

