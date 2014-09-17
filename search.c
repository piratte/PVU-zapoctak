#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <err.h>
#include <assert.h>

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
	if ((cur = readln(cur, fptr)) == NULL) {
		return (true);
	}

	next = readln(next, fptr);

	/* pattern found */
	if ((res = strstr(cur, needle)) != NULL) {
		// fprintf(stderr, "found on line 1\n");
		out = addres(out, NULL, cur, next);
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
		fprintf(fout, "%s", out);
		pthread_mutex_unlock(outmut);
	}
	free(out); free(cur); free(next); free(prev); free(tmp);
	return (true);
}

/* kdyz je -l zvoleno, prida to k hledane radce jeste predchozi a nasledujici */
char *
addres(char *out, char *prev, char *cur, char *next) {
	// fprintf(stderr, "cur: %s\n", cur);
	if (loutput) {
		if (prev)
			out = join(out, prev);
		out = join(out, cur);
		if (next)
			out = join(out, next);
		out = join(out, "---\n");
		// fprintf(stderr,"after joins: %s\n", out);
	} else {
		out = join(out, cur);
		// fprintf(stderr,"after join: %s\n", out);
	}
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

bool msearch(char *filename) {
	char *out, *soub;
	char *prev, *curst, *curend, *next, *end;
	int fin, fend;
	int ind, found, len;
	bool doprint = false;
	// fprintf(stderr, "in msearch: %s\n", filename);

	out = malloc(strlen("searching "));
	out = "searching ";
	out = join(out, filename);
	out = join(out, ": \n");

	len = strlen(needle);

	if (!(fin = open(filename, O_RDONLY))) {
		perror(filename);
		return (false);
	}

	fend = lseek(fin, 0, SEEK_END);

	soub = mmap(0, fend, PROT_READ, MAP_PRIVATE, fin, 0);
	if (soub == (void*) -1) {
		// perror(filename);
		close(fin);
		return (false);
	}

	found = 0; ind = 0;
	end = soub + fend;
	/* finding newlines */
	prev = NULL; curst = soub -1;
	curend = strchr(curst + 1, '\n');
	if (curend != NULL)
		next = strchr(curend + 1, '\n');
	else
		next = end;

	// fprintf(stderr, "before while\n");

	// fprintf(stderr, "fend for %s is %d\n", filename, fend);
	/* main searching loop */
	while (ind < fend) {
		// fprintf(stderr, "loop iteration on %s\n", filename);
		if (needle[found] == soub[ind]) {
			++found;
			++ind;
			if (found == len) {
				// fprintf(stderr, "found\n");
				doprint = true;
				// fprintf(stderr, "before mjoin\n");
				/* adding found line(s) to output buffer */
				if (loutput) {
					if (prev == NULL)
						prev = curst;
					if (next == NULL)
						next = curend;
					out = mjoin(out, prev + 1, next);
				}
				else
					out = mjoin(out, curst + 1, curend);
				found = 0;

				/* rotating newline pointers */

				prev = curst;
				curst = curend;
				curend = next;
				if ((next != end) && (curend != NULL))
					if ((next = strchr(curend + 1, '\n'))
						== NULL)
						next = end;
				// fprintf(stderr, "after rotating\n");
				/* updating searching pos */
				ind += curst - &soub[ind];
				continue;
			}
		} else /* needle[found] != soub[ind] */ {
			if (found != 0)
				found = zpet[found-1];
			else
				++ind;
		}
		if (soub[ind] == '\n') {
			prev = curst;
			curst = curend;
			curend = next;
			if (next + 1 == end)
				continue;
			if ((next != end) && (curend != NULL))
					if ((next = strchr(curend + 1, '\n'))
						== NULL)
						next = end;
		}

	}

	munmap(soub, fend);
	close(fin);
	if (doprint) {
		out = join(out, "==========================================\n");
		pthread_mutex_lock(outmut);
		fprintf(fout, "%s", out);
		pthread_mutex_unlock(outmut);
	}
	return (true);
}

char *
mjoin(const char *out, char *start, const char *end) {
	int outend = strlen(out);
	int i = outend;
	char *index, *result;

	if (loutput)
		result = malloc(outend + sizeof (char) * (end - start) + 7);
	else
		result = malloc(outend + sizeof (char) * (end - start) + 2);
	// fprintf(stderr, "%d\n", (end - start));
	strcpy(result, out);
	for (index = start; index < end; ++index)
		result[i++] = *index;

	if (loutput) {
		result[i++] = '\n';
		result[i++] = '-';
		result[i++] = '-';
		result[i++] = '-';
		result[i++] = '-';
	}
	result[i++] = '\n';
	result[i++] = 0;
	// fprintf(stderr, "after mjoin: %s\n", result);
	return (result);
}