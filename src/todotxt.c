#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <unistd.h>

#include "tasks.h"
#include "todotxt.h"

#define MAX_LINES_IN_FILE 1000
#define MAX_LINE_LENGTH 1024

TodoTxt *_create_todotxt(char *filename);

Task *_read_task(char *line);
int _read_completed(char *line, Task *dest);
int _read_completion_date(char *line, Task *dest);
int _read_priority(char *line, Task *dest);
int _read_creation_date(char *line, Task *dest);
int _read_description(char *line, Task *dest);
int _read_date(char *line, time_t *dest);

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
	if (EOF == fclose(file)) return 0;

	todo->len = len;
	todo->lines = lines;
	return todo;
}

TodoTxt *_create_todotxt(char *filename) {
	TodoTxt *todo = malloc(sizeof(TodoTxt));
	todo->len = 0;
	todo->lines = 0;
	todo->filename = malloc(strlen(filename) + 1);
	strcpy(todo->filename, filename);
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
	TaskList *list = create_tasklist(todo->len);
	for (int i = 0; i < todo->len; ++i) {
		char *line = todo->lines[i];
		list->tasks[i] = _read_task(line);
	}
	list->len = todo->len;
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
		if (is_prioritized(task)) {
			buffer[0] = '('; buffer[1] = task->priority; buffer[2] = ')'; buffer[3] = ' ';
			position = 4;
		}
		strcpy(&buffer[position], task->description);
		fputs(buffer, fh); fputs("\n", fh);
		lines[i] = malloc(strlen(buffer) + 1);
		strcpy(lines[i], buffer);
	}
	if (EOF == fclose(fh)) return 0;
	todo->lines = lines;
	return todo->len;
}

int read_lines(FILE *file, char **lines) {
	char *buffer = malloc(MAX_LINE_LENGTH);
	int count = 0;
	while (! feof(file)) {
		memset(buffer, 0, MAX_LINE_LENGTH);
		fgets(buffer, MAX_LINE_LENGTH, file);
		if (_empty_line(buffer)) continue;

		_strip_lineend(buffer);
		char *line =  malloc(strlen(buffer) + 1);
		strcpy(line, buffer);
		lines[count] = line;
		count++;
	}
	return count;
}

Task *_read_task(char *line) {
	Task *t = create_empty_task();
	int position = 0;
	position += _read_completed(&line[position], t);
	if (is_completed(t)) {
		position += _read_completion_date(&line[position], t);
	}
	if (! is_completed(t)) {
		position += _read_priority(&line[position], t);
	}
	position += _read_creation_date(&line[position], t);
	position += _read_description(&line[position], t);
	return t;
}

int _read_completed(char *line, Task *dest) {
	if (line[0] == 'x' && line[1] == ' ') {
		dest->completed = 1;
		return 2;
	} else {
		dest->completed = 0;
		return 0;
	}
}

int _read_completion_date(char *line, Task *dest) {
	time_t *time = malloc(sizeof(time_t));
	int len_read = _read_date(line, time);
	if (len_read) {
		dest->completion_date = time;
		return len_read + 1;
	} else {
		free(time);
		return 0;
	}
}

int _read_priority(char *line, Task *dest) {
	int line_has_priority = strlen(line) > 4
						&& line[0] == '('
						&& line[1] >= 'A' && line[1] <= 'Z'
						&& line[2] == ')'
						&& line[3] == ' ';
	if (line_has_priority) {
		dest->priority = line[1];
		return 4;
	} else {
		return 0;
	}
}

int _read_creation_date(char *line, Task *dest) {
	time_t *time = malloc(sizeof(time_t));
	int len_read = _read_date(line, time);
	if (len_read > 0) {
		dest->creation_date = time;
		return len_read + 1;
	} else {
		free(time);
	}
	return 0;
}

int _read_description(char *line, Task *dest) {
	int len = strlen(line) + 1;
	dest->description = malloc(len);
	strcpy(dest->description, line);
	return len;
}

int _read_date(char *line, time_t *dest) {
	struct tm tm;
	char *endOfDate = strptime(line, "%Y-%m-%d", &tm);
	if (NULL != endOfDate) {
		tm.tm_hour = 0;
		tm.tm_min = 0;
		tm.tm_sec = 0;
		tm.tm_isdst = 0;
		time_t time = mktime(&tm);
		memcpy(dest, &time, sizeof(time_t));
		return endOfDate - line;
	}
	return 0;
}

int _fexists(char *filename) {
	return 0 == access(filename, F_OK);
}

int _empty_line(char *line) {
	return 0 == strlen(line) || 0 == strcmp(line, "\n");
}

void _strip_lineend(char *line) {
	char *lineend = strchr(line, '\n');
	if (NULL != lineend) {
		int index = (int)(lineend - line);
		line[index] = '\0';
	}
}

