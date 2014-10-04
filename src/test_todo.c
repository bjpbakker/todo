#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <assert.h>

#include "tasks.h"
#include "todotxt.h"

#define run_test(fn_name)\
	printf("%s\n", #fn_name);\
	fn_name();


char *_create_tmpfile(char *template);
time_t *_delta_time();
time_t *_current_time();
char *_format_date(time_t *time);

void test_todotxt_open() {
	char *tmpfile = _create_tmpfile("/tmp/todo.XXXXXXX");
	FILE *fh = fopen(tmpfile, "w");
	assert(fh);
	fputs("32 bytes don't overlap with next\n", fh);
	fputs("next line\n", fh);
	assert(EOF != fclose(fh));

	TodoTxt *todo = todotxt_open(tmpfile);
	assert(0 == strcmp("32 bytes don't overlap with next", todo->lines[0]));
	assert(0 == strcmp("next line", todo->lines[1]));
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
	assert(0 == strcmp(list->tasks[0]->description, "One Task"));
	free_tasklist(list);
}

void test_todotxt_read_completed_task() {
	TodoTxt todo;
	todo.len = 4;
	todo.lines = malloc(todo.len * sizeof(char*));
	todo.lines[0] = "x 2014-02-22 task";
	todo.lines[1] = "xylophone";
	todo.lines[2] = "X task";
	todo.lines[3] = "x task";

	TaskList *list = todotxt_read_tasklist(&todo);
	assert(is_completed(list->tasks[0]));
	assert(0 == strcmp("02/22/2014", _format_date(list->tasks[0]->completion_date)));
	assert(0 == strcmp("task", list->tasks[0]->description));
	assert(! is_completed(list->tasks[1]));
	assert(! is_completed(list->tasks[2]));
	assert(is_completed(list->tasks[3]));
	assert(0 == strcmp("task", list->tasks[3]->description));
	free_tasklist(list);
}

void test_todotxt_read_task_with_priority() {
	TodoTxt todo;
	todo.len = 4;
	todo.lines = malloc(todo.len * sizeof(char*));
	todo.lines[0] = "(A) Task";
	todo.lines[1] = "(A)-> Task";
	todo.lines[2] = "(a) Task";
	todo.lines[3] = "x 2014-07-22 (A) Task";

	TaskList *list = todotxt_read_tasklist(&todo);
	assert(list->tasks[0]->priority == 'A');
	assert(0 == strcmp(list->tasks[0]->description, "Task"));
	assert(is_unprioritized(list->tasks[1]));
	assert(is_unprioritized(list->tasks[2]));
	assert(is_unprioritized(list->tasks[3]));
	free_tasklist(list);
}

void test_todotxt_read_task_with_creation_date() {
	TodoTxt todo;
	todo.len = 1;
	todo.lines = malloc(todo.len * sizeof(char*));
	todo.lines[0] = "2014-07-22 Task";

	TaskList *list = todotxt_read_tasklist(&todo);
	assert(0 == strcmp("07/22/2014", _format_date(list->tasks[0]->creation_date)));
	assert(0 == strcmp("Task", list->tasks[0]->description));
	free_tasklist(list);
}

void test_todotxt_write_tasks() {
	char *tmpfile = _create_tmpfile("/tmp/todo.XXXXXXX");
	TodoTxt *todo = todotxt_open(tmpfile);
	assert(todo);

	Task *task = create_task("New Task");
	Task *prioritized = create_prioritized_task("Prioritized Task", 'A');

	TaskList *tasks = create_tasklist(2);
	tasks->len = 2;
	tasks->tasks[0] = task;
	tasks->tasks[1] = prioritized;

	int written = todotxt_write_tasklist(todo, tasks);
	assert(2 == written);
	todotxt_close(todo);

	todo = todotxt_open(tmpfile);
	assert(todo != 0);

	assert(todo->len == 2);
	assert(0 == strcmp(todo->lines[0], "New Task"));
	assert(0 == strcmp(todo->lines[1], "(A) Prioritized Task"));

	todotxt_close(todo);
	free_tasklist(tasks);
	free(tmpfile);
}

void test_copy_task() {
	Task *proto = create_prioritized_task("proto", 'C');
	proto->creation_date = _current_time();
	proto->completed = 1;
	proto->completion_date = _current_time();

	Task *copy = copy_task(proto);
	assert('C' == copy->priority);
	assert(0 == strcmp("proto", copy->description));
	assert(is_completed(copy));
	assert(*proto->creation_date == *copy->creation_date);
	assert(*proto->completion_date == *copy->completion_date);

	free_task(proto);
	free_task(copy);
}

void test_prioritized() {
	Task *prioritized = create_prioritized_task("task", 'A');
	assert(is_prioritized(prioritized));
	assert(! is_unprioritized(prioritized));

	Task *unprioritized = create_task("task");
	assert(! is_prioritized(unprioritized));
	assert(is_unprioritized(unprioritized));

	free_task(prioritized);
	free_task(unprioritized);
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
	tasklist_sort(list, sorted, by_priority);

	assert(0 == strcmp("ONE", sorted->tasks[0]->description));
	assert(0 == strcmp("TWO", sorted->tasks[1]->description));
	assert(0 == strcmp("THREE", sorted->tasks[2]->description));

	free_tasklist(list);
	free_tasklist(sorted);
}

void test_sort_by_default() {
	Task *one = create_prioritized_task("ONE", 'A');
	Task *two = create_task("TWO");
	Task *three = create_task("THREE");
	three->completed = 1;
	three->completion_date = _current_time();

	TaskList *list = create_tasklist(3);
	list->tasks[0] = three;
	list->tasks[1] = two;
	list->tasks[2] = one;
	list->len = 3;

	TaskList *sorted = create_tasklist(3);
	tasklist_sort(list, sorted, by_default);

	assert(0 == strcmp("ONE", sorted->tasks[0]->description));
	assert(0 == strcmp("TWO", sorted->tasks[1]->description));
	assert(0 == strcmp("THREE", sorted->tasks[2]->description));

	free_tasklist(list);
	free_tasklist(sorted);
}

void test_days_taken_to_complete() {
	Task *completed_immediately = create_task("t");
	completed_immediately->creation_date = _current_time();
	completed_immediately->completion_date = _current_time();
	assert(0 == days_taken_to_complete(completed_immediately));

	Task *took_one_day = create_task("t");
	took_one_day->creation_date = _delta_time(-1);
	took_one_day->completion_date = _current_time();
	assert(1 == days_taken_to_complete(took_one_day));
}

char *_create_tmpfile(char *template) {
	size_t size = strlen(template) + 1;
	char *tmpfile = malloc(size);
	memset(tmpfile, 0, size);
	strncat(tmpfile, template, size);
	mkstemp(tmpfile);
	return tmpfile;
}

time_t *_delta_time(int d_days) {
	time_t *t = malloc(sizeof(time_t));
	time_t now = time(NULL) + d_days * 24 * 60 * 60;
	memcpy(t, &now, sizeof(time_t));
	return t;
}

time_t *_current_time() {
	return _delta_time(0);
}

char *_format_date(time_t *time) {
	char *formatted = malloc(10);
	memset(formatted, 0, 10);
	struct tm *tm = localtime(time);
	strftime(formatted, 10, "%m/%d/%Y", tm);
	return formatted;
}

int main() {
	run_test(test_todotxt_open);
	run_test(test_todotxt_open_with_non_existing_file);
	run_test(test_todotxt_read_tasks);
	run_test(test_todotxt_read_completed_task);
	run_test(test_todotxt_read_task_with_priority);
	run_test(test_todotxt_read_task_with_creation_date);
	run_test(test_todotxt_write_tasks);
	run_test(test_copy_task);
	run_test(test_prioritized);
	run_test(test_sort_by_priority);
	run_test(test_sort_by_default);
	run_test(test_days_taken_to_complete);
	printf("\nOK\n");
	return 0;
}

