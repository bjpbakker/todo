#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

#include "tasks.h"
#include "todotxt.h"

#define run_test(fn_name)\
	printf("%s\n", #fn_name);\
	fn_name();


char *_create_tmpfile(char *template);

void test_todotxt_open() {
	TodoTxt *todo = todotxt_open("examples/todo.txt");
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
	assert(is_unprioritized(list->tasks[0]));
	assert(strcmp(list->tasks[0]->description, "One Task") == 0);
	free_tasklist(list);
}

void test_todotxt_read_task_with_priority() {
	TodoTxt todo;
	todo.len = 3;
	todo.lines = malloc(sizeof(char*));
	todo.lines[0] = "(A) Task";
	todo.lines[1] = "(A)-> Task";
	todo.lines[2] = "(a) Task";

	TaskList *list = todotxt_read_tasklist(&todo);
	assert(list->tasks[0]->priority == 'A');
	assert(strcmp(list->tasks[0]->description, "Task") == 0);
	assert(is_unprioritized(list->tasks[1]));
	assert(is_unprioritized(list->tasks[2]));
	free_tasklist(list);
}
	free_tasklist(list);
}

void test_todotxt_write_tasks() {
	char *tmpfile = _create_tmpfile("/tmp/todo.XXXXXXX");
	TodoTxt *todo = todotxt_open(tmpfile);
	assert(todo != 0);

	Task *task = create_task("New Task");
	Task *prioritized = create_prioritized_task("Prioritized Task", 'A');

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

void test_prioritized() {
	Task *prioritized = create_prioritized_task("task", 'A');
	assert(is_prioritized(prioritized));
	assert(! is_unprioritized(prioritized));

	Task *unprioritized = create_task("task");
	assert(! is_prioritized(unprioritized));
	assert(is_unprioritized(unprioritized));
}

void test_sort_by_priority() {
	Task *one = create_prioritized_task("ONE", 'A');
	Task *two = create_prioritized_task("TWO", 'B');
	Task *three = create_task("THREE");

	TaskList *list = create_tasklist(3);
	list->tasks[0] = two;
	list->tasks[1] = three;
	list->tasks[2] = one;
	list->len = 3;

	TaskList *sorted = create_tasklist(3);
	tasklist_sort_by_priority(list, sorted);

	assert(0 == strcmp("ONE", sorted->tasks[0]->description));
	assert(0 == strcmp("TWO", sorted->tasks[1]->description));
	assert(0 == strcmp("THREE", sorted->tasks[2]->description));

	free_tasklist(list);
	free_tasklist(sorted);
}

char *_create_tmpfile(char *template) {
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
	run_test(test_prioritized);
	run_test(test_sort_by_priority);
	printf("\nOK\n");
	return 0;
}

