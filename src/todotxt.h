#ifndef TODOTXT_H
#define TODOTXT_H

#include "tasks.h"

struct TodoTxt;
typedef struct TodoTxt {
	char* filename;
	int len;
	char **lines;
} TodoTxt;

TodoTxt *todotxt_open(char *filename);
void todotxt_close(TodoTxt *todo);

TaskList *todotxt_read_tasklist(TodoTxt *todo);
int todotxt_write_tasklist(TodoTxt *todo, TaskList *tasks);

int read_lines(FILE *file, char **lines);

#endif

