#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "todotxt.h"

int main(void) {
	TodoTxt *todo = todotxt_open("todo.txt");
	if (!todo) { puts("Cannot open todo.txt"); return 1; }

	TaskList *list = todotxt_read_tasklist(todo);

	size_t size = sizeof(Task*) * list->len;
	Task **tasks = malloc(size);
	memcpy(tasks, list->tasks, size);
	qsort(tasks, list->len, sizeof(Task*), by_priority);

	for (int i = 0; i < list->len; ++i) {
		Task *t = tasks[i];

		char *display = malloc(strlen(t->description) + 3);
		if ('\0' != t->priority) {
			display[0] = t->priority; display[1] = ':'; display[2] = ' ';
		} else {
			strcat(display, "   ");
		}
		strcpy(&display[3], t->description);

		puts(display);
	}
	free(tasks);
	free_tasklist(list);
	todotxt_close(todo);
}
