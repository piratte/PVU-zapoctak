#ifndef __SEARCH_H
#define	__SEARCH_H

#include <stdio.h>

bool search(char *filename);
bool readln(char *line, FILE *stream);
char *addres(char *out, char *prev, char *cur, char *next);

char *join(const char *s1, const char *s2);

#endif /* __SEARCH_H */