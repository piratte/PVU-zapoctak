#ifndef __SEARCH_H
#define	__SEARCH_H

#include <stdio.h>

bool search(char *filename);
char *readln(char *line, FILE *stream);
char *addres(char *out, char *prev, char *cur, char *next);

char *join(const char *s1, const char *s2);

bool msearch(char *filename);
char *mjoin(const char *out, char *start, const char *end);

#endif /* __SEARCH_H */