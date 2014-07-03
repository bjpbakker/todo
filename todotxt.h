#ifndef TODOTXT_H
#define TODOTXT_H

#include "todo.h"

struct TodoTxt;
typedef struct TodoTxt TodoTxt;

TodoTxt *todotxt_open(char *filename);
void todotxt_close(TodoTxt *todo);

TaskList *todotxt_read_tasklist(TodoTxt *todo);
int todotxt_write_tasklist(TodoTxt *todo, TaskList *tasks);

int read_lines(FILE *file, char **lines);

#endif

