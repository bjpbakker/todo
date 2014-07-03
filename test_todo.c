#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

#include "tasks.h"
#include "todotxt.h"

#define run_test(fn_name)\
	printf("%s\n", #fn_name);\
	fn_name();


typedef struct TodoTxt {
	void* _;
	int len;
	char **lines;
} TodoTxt;

char *create_tmpfile(char *template);

void test_todotxt_open() {
	TodoTxt *todo = todotxt_open("todo.txt");
	assert(todo != 0);
	assert(todo->len > 0);
	assert(strcmp(todo->lines[0], "") != 0);
	todotxt_close(todo);
}

void test_todotxt_open_with_non_existing_file() {
	TodoTxt *todo = todotxt_open("new-file.txt");
	assert(todo != 0);
	assert(todo->len == 0);
	todotxt_close(todo);
}

void test_todotxt_read_tasks() {
	TodoTxt todo;
	todo.len = 1;
	todo.lines = malloc(sizeof(char*));
	todo.lines[0] = "One Task";

	TaskList *list = todotxt_read_tasklist(&todo);
	assert(list->len == 1);
	assert(list->tasks[0]->priority == '\0');
	assert(strcmp(list->tasks[0]->description, "One Task") == 0);
	free_tasklist(list);
}

void test_todotxt_read_task_with_priority() {
	TodoTxt todo;
	todo.len = 2;
	todo.lines = malloc(sizeof(char*));
	todo.lines[0] = "(A) Task";
	todo.lines[1] = "(A)-> Task";

	TaskList *list = todotxt_read_tasklist(&todo);
	assert(list->tasks[0]->priority == 'A');
	assert(strcmp(list->tasks[0]->description, "Task") == 0);
	assert(list->tasks[1]->priority == '\0');
	free_tasklist(list);
}

void test_todotxt_write_tasks() {
	char *tmpfile = create_tmpfile("/tmp/todo.XXXXXXX");
	TodoTxt *todo = todotxt_open(tmpfile);
	assert(todo != 0);

	Task *task = create_task("New Task");
	Task *prioritized = create_task("Prioritized Task");
	prioritized->priority = 'A';

	TaskList *tasks = create_tasklist(2);
	tasks->len = 2;
	tasks->tasks[0] = task;
	tasks->tasks[1] = prioritized;

	int written = todotxt_write_tasklist(todo, tasks);
	assert(written);
	todotxt_close(todo);

	FILE *fh = fopen(tmpfile, "r");
	assert(fh);
	char **lines = malloc(20 * sizeof(char*));
	memset(lines, 0, 2 * sizeof(char*));
	int len_written = read_lines(fh, lines);

	assert(len_written == 2);
	assert(0 == strcmp(lines[0], "New Task"));
	assert(0 == strcmp(lines[1], "(A) Prioritized Task"));

	free_tasklist(tasks);
	free(tmpfile);
}

void test_by_priority() {
	Task *one = create_task("ONE");
    one->priority = 'A';
	Task *two = create_task("TWO");
    two->priority = 'B';

	TaskList *list = create_tasklist(2);
	list->tasks[0] = two;
	list->tasks[1] = one;
	list->len = 2;

    qsort(list->tasks, 2, sizeof(Task*), by_priority);

    assert(list->tasks[0] == one);
    assert(list->tasks[1] == two);
}

char *create_tmpfile(char *template) {
	int len = strlen(template);
	char *tmpfile = malloc(len);
	strncat(tmpfile, template, len);
	mkstemp(tmpfile);
	return tmpfile;
}

int main() {
	run_test(test_todotxt_open);
	run_test(test_todotxt_open_with_non_existing_file);
	run_test(test_todotxt_read_tasks);
	run_test(test_todotxt_read_task_with_priority);
	run_test(test_todotxt_write_tasks);
    run_test(test_by_priority);
	printf("\nOK\n");
	return 0;
}

