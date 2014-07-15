#ifndef TASKS_H
#define TASKS_H

struct Task {
	char *description;
	char priority;
	int completed;
};
typedef struct Task Task;

struct TaskList {
	int len;
	Task **tasks;
};
typedef struct TaskList TaskList;

Task *create_task(char *description);
Task *create_prioritized_task(char *description, char priority);
Task *create_empty_task();
Task *copy_task(Task *proto);
void free_task(Task *task);

int is_prioritized(Task *task);
int is_unprioritized(Task *task);

TaskList *create_tasklist(int capacity);
void free_tasklist(TaskList *list);

void tasklist_sort_by_priority(TaskList *src, TaskList *dest);

#endif

