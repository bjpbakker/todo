#include <stdlib.h>
#include <string.h>

#include "tasks.h"

Task *create_task(char *description) {
	Task *t = malloc(sizeof(Task));
	t->description = description;
	t->priority = '\0';
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

int by_priority(const void *lhs, const void *rhs) {
	Task *t1 = *(Task**)lhs;
	Task *t2 = *(Task**)rhs;
	if (t1->priority == t2->priority) return 0;
    if (t1->priority == '\0') return 1;
    if (t2->priority == '\0') return -1;
    if (t1->priority < t2->priority) return -1;
    if (t1->priority > t2->priority) return 1;
}

