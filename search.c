#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
// #include <readline/readline.h>

#include "search.h"

#define MAXLINE 512

bool search(char *filename, char *needle) {

	FILE *fptr;
	char *prev, *cur, *next, *tmp;
	char *res, *out;

	/* allocating mem for reading lines */
	cur = (char*) calloc(MAXLINE, sizeof(char));
	next = (char*) calloc(MAXLINE, sizeof(char));
	tmp = (char*) calloc(MAXLINE, sizeof(char));
	prev = (char*) calloc(MAXLINE, sizeof(char));

	/* memory for output string */
	out = malloc(strlen("searching "));
	out = "searching ";

	out = join(out, filename);
	out = join(out, ": \n");
	// test-write
	printf("%s",out);

	/* opening file */
	if (!(fptr = fopen(filename, "r"))) {
        perror(filename);
        return (false);
    }

    /* empty file */
    if (readln(cur, fptr) == false) {
    	return (true);
    }

    /* one line file test */
    if (readln(next, fptr) == false) {
    	next = NULL;
    }

    /* pattern found */
	if ((res = strstr(cur, needle)) != NULL) {
		addres(out, NULL, cur, NULL);
	}

	while(next != NULL) {
		tmp = prev;
		prev = cur;
		cur = next;
		next = tmp;

		if (readln(next, fptr) == false) 
    		next = NULL;
    
		if ((res = strstr(cur, needle)) != NULL) {
			addres(out, prev, cur, next);
		}
	}

	// TODO search through the last line

	//char *res = strstr();
	printf("%s\n==========================================\n", out);
	fclose(fptr);
	free(out);
	return (true);
}

/* kdyz je -l zvoleno, prida to k hledane radce jeste predchozi a nasledujici*/
void addres(char *out, char *prev, char *cur, char *next) {
	printf("found\n");
}

/* do *line nacte radku ze *stream a vrati false pokud neuspeje */
bool readln(char *line, FILE *stream){
	char *cptr;
	/* test write printf("rln\n"); */
	if ((cptr = fgets(line, MAXLINE, stream)) != NULL) {
		/* stripping white space
		while(*cptr == ' ' || *cptr == '\t') 
			cptr++;
		line = cptr; */
		return true;    
	} 
	return false;
}

char *join(const char* s1, const char* s2)
{
    char* result = malloc(strlen(s1) + strlen(s2) + 1);

    if (result)
    {
        strcpy(result, s1);
        strcat(result, s2);
    }

    return result;
}