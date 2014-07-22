default: all

all: compile test
again: clean all
.PHONY: all again

src/version.h: .git/*
	$(shell scripts/version-gen.sh > src/version.h)

compile: src/*.h src/*.c src/version.h
	mkdir -p build
	gcc --std=c99 -o build/todo src/todotxt.c src/tasks.c src/app.c
.PHONY: compile

test: build/.tests-pass
.PHONY: test
	
build/.tests-pass: src/*.h src/*.c
	mkdir -p build
	gcc --std=c99 -o build/test_todo src/todotxt.c src/tasks.c src/test_todo.c
	./build/test_todo
	touch $@

clean:
	rm -rf build/
	rm -f src/version.h
.PHONY: clean

