#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "tasks.h"

Task *create_task(char *description) {
	Task *t = malloc(sizeof(Task));
	memset(t, 0, sizeof(Task));
	t->description = malloc(strlen(description) + 1);
	strcpy(t->description, description);
	t->priority = '\0';
	return t;
}

Task *create_prioritized_task(char *description, char priority) {
	Task *t = create_task(description);
	t->priority = priority;
	return t;
}

Task *create_empty_task() {
	Task *t = malloc(sizeof(Task));
	memset(t, 0, sizeof(Task));
	return t;
}

void *_copy(void *proto, size_t size) {
	void *dest = malloc(size);
	memset(dest, 0, size);
	memcpy(dest, proto, size);
	return dest;
}

Task *copy_task(Task *proto) {
	Task *t = create_empty_task();
	t->priority = proto->priority;
	t->description = (char*) _copy(proto->description, strlen(proto->description) + 1);
	t->completed = proto->completed;
	if (proto->creation_date) {
		t->creation_date = _copy(proto->creation_date, sizeof(time_t));
	}
	if (proto->completion_date) {
		t->completion_date = _copy(proto->completion_date, sizeof(time_t));
	}
	return t;
}

void free_task(Task *task) {
	if (task->description != 0)
		free(task->description);
	if (task->creation_date)
		free(task->creation_date);
	if (task->completion_date)
		free(task->completion_date);
	free(task);
}

int is_completed(Task *task) {
	return task->completed;
}

int is_prioritized(Task *task) {
	return task->priority != '\0';
}

int is_unprioritized(Task *task) {
	return ! is_prioritized(task);
}

int days_taken_to_complete(Task *task) {
	double diff_in_sec = difftime(*task->completion_date, *task->creation_date);
	return diff_in_sec / 60 / 60 / 24;
}

TaskList *create_tasklist(int capacity) {
	TaskList *list = malloc(sizeof(TaskList));
	list->len = 0;
	int size = capacity * sizeof(Task*);
	list->tasks = malloc(size);
	memset(list->tasks, 0, size);
	return list;
}

void free_tasklist(TaskList *list) {
	for (int i = 0; i < list->len; ++i) {
		free_task(list->tasks[i]);
	}
	free(list->tasks);
	free(list);
}

void tasklist_sort(TaskList *src, TaskList *dest, int (*comp)(const void *lhs, const void *rhs)) {
	for (int i = 0; i < src->len; ++i) {
		Task *t = copy_task(src->tasks[i]);
		dest->tasks[i] = t;
	}
	dest->len = src->len;
	qsort(dest->tasks, dest->len, sizeof(Task*), comp);
}

int by_default(const void *lhs, const void *rhs) {
	Task *t1 = *(Task**)lhs;
	Task *t2 = *(Task**)rhs;
	if (is_completed(t1) && ! is_completed(t2)) return 1;
	if (! is_completed(t1) && is_completed(t2)) return -1;
	return by_priority(lhs, rhs);
}

int by_priority(const void *lhs, const void *rhs) {
	Task *t1 = *(Task**)lhs;
	Task *t2 = *(Task**)rhs;
	if (t1->priority == t2->priority) return 0;
	if (is_unprioritized(t2) || (is_prioritized(t1) && t1->priority < t2->priority)) return -1;
	return 1; // is_unprioritized(t1) || t1->priority > t2->priority
}
