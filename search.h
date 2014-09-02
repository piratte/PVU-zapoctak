#ifndef __SEARCH_H
#define __SEARCH_H

#include <stdio.h>

bool search(char *filename, char *needle, bool loutput);
bool readln(char *line, FILE *stream);
char *addres(char *out, char *prev, char *cur, char *next, bool loutput);

char *join(const char* s1, const char* s2);

#endif /* __SEARCH_H */