#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <getopt.h>

#include "tasks.h"
#include "todotxt.h"

char *_format_task_for_display(Task* t);

struct Options {
	char *file;
	int show_completed;
};
typedef struct Options Options;

Options *_parse_options(int argc, char **argv);
Options *_create_default_options();

void _usage();

int main(int argc, char **argv) {
	Options *options = _parse_options(argc, argv);

	TodoTxt *todo = todotxt_open(options->file);
	if (!todo) { puts("Cannot open todo.txt"); return 1; }

	TaskList *list = todotxt_read_tasklist(todo);
	TaskList *prioritized = create_tasklist(list->len);
	tasklist_sort_by_priority(list, prioritized);

	for (int i = 0; i < prioritized->len; ++i) {
		Task *task = prioritized->tasks[i];
		if (task->completed && ! options->show_completed) continue;

		char *formatted = _format_task_for_display(task);
		puts(formatted);
		free(formatted);

	}
	free_tasklist(prioritized);
	free_tasklist(list);
	todotxt_close(todo);
}

Options *_parse_options(int argc, char **argv) {
	Options *options = _create_default_options();
	int ch;
	while ((ch = getopt(argc, argv, "hf:x")) != -1) {
		switch (ch) {
			case 'f':
				options->file = optarg;
				break;
			case 'h':
				_usage();
				exit(0);
			case 'x':
				options->show_completed = 1;
				break;
			case '?':
			default:
				_usage();
				exit(1);
		}
	}

	return options;
}

Options *_create_default_options() {
	Options *options = malloc(sizeof(Options));
	memset(options, 0, sizeof(Options));
	options->file = "todo.txt";
	options->show_completed = 0;
	return options;
}

void _usage() {
	puts("usage: todo [OPTIONS]");
	puts("");
	puts("options:");
	puts("\t-f FILE\ttodotxt file to use (default: todo.txt)");
	puts("\t-x\tshow completed tasks");
	puts("");
}

char *_format_task_for_display(Task* t) {
	char *display = malloc(strlen(t->description) + 30);
	if (is_prioritized(t)) {
		display[0] = t->priority; display[1] = ':'; display[2] = ' ';
	} else {
		strcat(display, "   ");
	}
	strcat(display, t->description);
	if (t->completed) {
		strcat(display, " [done]");
	}

	return display;
}

