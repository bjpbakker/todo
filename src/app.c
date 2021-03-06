#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <getopt.h>

#include "tasks.h"
#include "todotxt.h"
#include "version.h"

char *_format_task_for_display(Task* t, Task* previous);
char *_format_date(time_t *time);
char *_format_days(int n);

struct Options {
	char *file;
	int show_completed;
};
typedef struct Options Options;

Options *_parse_options(int argc, char **argv);
Options *_create_default_options();

void _usage();
void _version();

int main(int argc, char **argv) {
	Options *options = _parse_options(argc, argv);

	TodoTxt *todo = todotxt_open(options->file);
	if (!todo) { puts("Cannot open todo.txt"); return 1; }

	TaskList *list = todotxt_read_tasklist(todo);
	TaskList *prioritized = create_tasklist(list->len);
	tasklist_sort(list, prioritized, by_default);

	Task* last_rendered = 0;
	for (int i = 0; i < prioritized->len; ++i) {
		Task *task = prioritized->tasks[i];
		if (is_completed(task) && ! options->show_completed) continue;

		char *formatted = _format_task_for_display(task, last_rendered);
		puts(formatted);
		free(formatted);
		last_rendered = task;

	}
	free_tasklist(prioritized);
	free_tasklist(list);
	todotxt_close(todo);
}

Options *_parse_options(int argc, char **argv) {
	Options *options = _create_default_options();
	int ch;
	while ((ch = getopt(argc, argv, "f:hvx")) != -1) {
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
			case 'v':
				_version();
				exit(0);
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
	puts("\t-v\tprint version info");
	puts("");
}

void _version() {
	if (VERSION_IS_RELEASE) {
		printf("todo, release %s\n", VERSION_GIT_TAG);
	} else {
		printf("todo, build %s\n", VERSION_GIT_REF);
	}
}

char *_format_task_for_display(Task* t, Task* previous) {
	int display_max_len = strlen(t->description) + 80;
	char *display = malloc(display_max_len);
	memset(display, 0, display_max_len);
    if (previous && is_prioritized(previous) && ! is_prioritized(t)) {
        strcat(display, "\n");
    }
	if (is_prioritized(t) && (! previous || previous->priority != t->priority)) {
		display[0] = t->priority; display[1] = ':'; display[2] = ' ';
	} else {
		strcat(display, "   ");
	}
	strcat(display, t->description);
	if (is_completed(t)) {
		strcat(display, " [done");
		if (t->completion_date) {
			char *completion_date = _format_date(t->completion_date);
			strcat(display, " "); strcat(display, completion_date);
			if (t->creation_date) {
				char *days = _format_days(days_taken_to_complete(t));
				strcat(display, ", took "); strcat(display, days); strcat(display, " to complete");
				free(days);
			}
			free(completion_date);
		}
		strcat(display, "]");
	} else if (t->creation_date && ! is_completed(t)) {
		char *creation_date = _format_date(t->creation_date);
		strcat(display, " [created "); strcat(display, creation_date); strcat(display, "]");
		free(creation_date);
	}

	return display;
}

char *_format_date(time_t *time) {
	char *date_str = malloc(10);
	memset(date_str, 0, 10);
	struct tm *tm = localtime(time);
	strftime(date_str, 10, "%D", tm);
	return date_str;
}

char *_format_days(int n) {
	char *days = malloc(8);
	int len = sprintf(days, "%d", n);
	char *formatted = malloc(len + 6);
	memset(formatted, 0, 6);
	strncat(formatted, days, len); strcat(formatted, " ");
	if (n == 1) {
		strcat(formatted, "day");
	} else {
		strcat(formatted, "days");
	}
	free(days);
	return formatted;
}

