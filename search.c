#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "search.h"
#include "main.h"

#define	DEFAULT_LINE 200

bool
search(char *filename) {

	FILE *fptr;
	char *prev, *cur, *next, *tmp;
	char *res, *out;
	bool found = false;
	// fprintf(stderr,"searching %s for %s\n", filename, needle );

	/* allocating mem for reading lines */
	cur = (char *) calloc(DEFAULT_LINE, sizeof (char));
	next = (char *) calloc(DEFAULT_LINE, sizeof (char));
	tmp = (char *) calloc(DEFAULT_LINE, sizeof (char));
	prev = (char *) calloc(DEFAULT_LINE, sizeof (char));

	/* memory for output string */
	out = malloc(strlen("searching "));
	out = "searching ";

	out = join(out, filename);
	out = join(out, ": \n");

	/* opening file */
	if (!(fptr = fopen(filename, "r"))) {
		perror(filename);
		return (false);
	}

	/* empty file */
	if ((next = readln(next, fptr)) == NULL) {
		return (true);
	}

	next = readln(next, fptr);

	/* pattern found */
	if ((res = strstr(cur, needle)) != NULL) {
		addres(out, NULL, cur, NULL);
		found = true;
	}

	while (next != NULL) {
		tmp = prev;
		prev = cur;
		cur = next;
		next = tmp;

		next = readln(next, fptr);
		// fprintf(stderr, "cur: %s\n", cur);

		if ((res = strstr(cur, needle)) != NULL) {
			out = addres(out, prev, cur, next);
			found = true;
			// fprintf(stderr,"after adress: %s\n", out);
		}
	}

	fclose(fptr);
	if (found) {
		out = join(out, "==========================================\n");
		pthread_mutex_lock(outmut);
		fprintf(fout,"%s", out);
		pthread_mutex_unlock(outmut);
	}
	free(out); free(cur); free(next); free(prev); free(tmp);
	return (true);
}

/* kdyz je -l zvoleno, prida to k hledane radce jeste predchozi a nasledujici */
char *
addres(char *out, char *prev, char *cur, char *next) {
	if (loutput) {
		if (prev)
			out = join(out, prev);
		out = join(out, cur);
		if (next)
			out = join(out, next);
		out = join(out, "---\n");
		// fprintf(stderr,"after joins: %s\n", out);
	}
	else
		out = join(out, cur);
	return (out);
}

/* do *line nacte radku ze *stream a vrati false pokud neuspeje */

char *
readln(char *line, FILE *stream) {
	int n;
	size_t lines = strlen(line);
	if ((n = getline(&line, &lines, stream)) != -1) {
		return (line);
	}
	else
		return (NULL);
}

char *
join(const char *s1, const char *s2)
{
	char *result = malloc(strlen(s1) + strlen(s2) + 1);

	if (result) {
		strcpy(result, s1);
		strcat(result, s2);
	}
	// fprintf(stderr,"join returning: %s\n",result );
	return (result);
}