#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tasks.h"
#include "todotxt.h"

char *_format_task_for_display(Task* t);

int main(void) {
	TodoTxt *todo = todotxt_open("todo.txt");
	if (!todo) { puts("Cannot open todo.txt"); return 1; }

	TaskList *list = todotxt_read_tasklist(todo);
	TaskList *prioritized = create_tasklist(list->len);
	tasklist_sort_by_priority(list, prioritized);

	for (int i = 0; i < prioritized->len; ++i) {
		char *formatted = _format_task_for_display(prioritized->tasks[i]);
		puts(formatted);
		free(formatted);

	}
	free_tasklist(prioritized);
	free_tasklist(list);
	todotxt_close(todo);
}

char *_format_task_for_display(Task* t) {
	char *display = malloc(strlen(t->description) + 3);
	if ('\0' != t->priority) {
		display[0] = t->priority; display[1] = ':'; display[2] = ' ';
	} else {
		strcat(display, "   ");
	}
	strcpy(&display[3], t->description);

	return display;
}

