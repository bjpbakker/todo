#include <stdlib.h>
#include <string.h>

#include "tasks.h"

int _by_priority(const void *lhs, const void *rhs);

Task *create_task(char *description) {
	Task *t = malloc(sizeof(Task));
	t->description = description;
	t->priority = '\0';
	return t;
}

Task *create_prioritized_task(char *description, char priority) {
	Task *t = create_task(description);
	t->priority = priority;
	return t;
}

Task *copy_task(Task *proto) {
	Task *t = malloc(sizeof(Task));
	memcpy(t, proto, sizeof(Task));
	return t;
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
		free(list->tasks[i]);
	}
	free(list->tasks);
	free(list);
}

void tasklist_sort_by_priority(TaskList *src, TaskList *dest) {
	for (int i = 0; i < src->len; ++i) {
		Task *t = copy_task(src->tasks[i]);
		dest->tasks[i] = t;
	}
	dest->len = src->len;
	qsort(dest->tasks, dest->len, sizeof(Task*), _by_priority);
}

int _by_priority(const void *lhs, const void *rhs) {
	Task *t1 = *(Task**)lhs;
	Task *t2 = *(Task**)rhs;
	if (t1->priority == t2->priority) return 0;
	if (t1->priority == '\0') return 1;
	if (t2->priority == '\0') return -1;
	if (t1->priority < t2->priority) return -1;
	if (t1->priority > t2->priority) return 1;
}

