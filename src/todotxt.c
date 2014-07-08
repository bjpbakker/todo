#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "tasks.h"
#include "todotxt.h"

#define MAX_LINES_IN_FILE 1000
#define MAX_LINE_LENGTH 1024

TodoTxt *_create_todotxt(char *filename);

void _parse_task(char *line, Task *dest);
int _line_has_priority(char *line);
int _fexists(char *filename);

int _empty_line(char *line);
void _strip_lineend(char *line);

TodoTxt *todotxt_open(char *filename) {
	TodoTxt *todo = _create_todotxt(filename);
	if (! _fexists(filename)) {
		return todo;
	}

	FILE *file = fopen(filename, "r");
	if (!file) return 0;

	int size = MAX_LINES_IN_FILE * sizeof(char*);
	char **lines = malloc(size);
	memset(lines, 0, size);
	int len = read_lines(file, lines);

	todo->len = len;
	todo->lines = lines;
	return todo;
}

TodoTxt *_create_todotxt(char *filename) {
	TodoTxt *todo = malloc(sizeof(TodoTxt));
	todo->len = 0;
	todo->lines = 0;
	todo->filename = malloc(strlen(filename));
	strcat(todo->filename, filename);
	return todo;
}

void todotxt_close(TodoTxt *todo) {
	free(todo->filename);
	if (0 != todo->lines) {
		free(todo->lines);
	}
	free(todo);
}

TaskList *todotxt_read_tasklist(TodoTxt *todo) {
	TaskList *list = malloc(sizeof(TaskList));
	list->len = todo->len;
	list->tasks = malloc(list->len * sizeof(void*));
	for (int i = 0; i < todo->len; ++i) {
		Task *t = malloc(sizeof(Task));
		_parse_task(todo->lines[i], t);
		list->tasks[i] = t;
	}
	return list;
}

int todotxt_write_tasklist(TodoTxt *todo, TaskList *tasks) {
	FILE *fh = fopen(todo->filename, "w");
	if (!fh) return 0;

	todo->len = tasks->len;
	char **lines = malloc(tasks->len);
	char *buffer = malloc(MAX_LINE_LENGTH);
	for (int i = 0; i < tasks->len; ++i) {
		memset(buffer, 0, MAX_LINE_LENGTH);
		int position = 0;
		Task *task = tasks->tasks[i];
		if ('\0' != task->priority) {
			buffer[0] = '('; buffer[1] = task->priority; buffer[2] = ')'; buffer[3] = ' ';
			position = 4;
		}
		strcpy(&buffer[position], task->description);
		fputs(buffer, fh); fputs("\n", fh);
		lines[i] = malloc(strlen(buffer));
		strcpy(lines[i], buffer);
	}
	fclose(fh);
	todo->lines = lines;
	return 1;
}

int read_lines(FILE *file, char **lines) {
	char *buffer = malloc(MAX_LINE_LENGTH);
	int count = 0;
	while (! feof(file)) {
		memset(buffer, 0, MAX_LINE_LENGTH);
		fgets(buffer, MAX_LINE_LENGTH, file);
		if (_empty_line(buffer)) continue;

		_strip_lineend(buffer);
		lines[count] = malloc(strlen(buffer));
		strcpy(lines[count], buffer);
		count++;
	}
	return count;
}

void _parse_task(char *line, Task *dest) {
	if (_line_has_priority(line)) {
		dest->priority = line[1];
		dest->description = malloc(strlen(line) - 4);
		strcat(dest->description, &line[4]);
	} else {
		dest->priority = '\0';
		dest->description = line;
	}
}

int _line_has_priority(char *line) {
    return strlen(line) > 4
        && line[0] == '('
        && line[1] >= 'A' && line[1] <= 'Z'
        && line[2] == ')'
        && line[3] == ' ';
}

int _fexists(char *filename) {
	return 0 == access(filename, F_OK);
}

int _empty_line(char *line) {
	return 0 == strlen(line) || 0 == strcmp(line, "\n");
}

void _strip_lineend(char *line) {
	char *lineend = strchr(line, '\n');
	if (0 != lineend) {
		int index = (int)(lineend - line);
		line[index] = '\0';
	}
}
