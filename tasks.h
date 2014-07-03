#ifndef TASKS_H
#define TASKS_H

struct Task {
	char priority;
	char *description;
};
typedef struct Task Task;

struct TaskList {
	int len;
	Task **tasks;
};
typedef struct TaskList TaskList;

Task *create_task(char *description);
TaskList *create_tasklist(int capacity);
void free_tasklist(TaskList *list);

int by_priority(const void *lhs, const void *rhs);

#endif

