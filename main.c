#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#include <errno.h>
#include <err.h>
#include <memory.h>

#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "explor.h"
#include "main.h"
#include "search.h"


int
main(int argc, char *argv[])
{
	int i, optc, erro;
	char c = ' ';
	char *h = "-h";
	char *infile = NULL;
	struct stat stats;

	if (argc < 3) {
		if ((argc == 2) && (strstr(argv[1], h) != NULL)) {
			print_usage(argv[0]);
			exit(0);
		}
		errx(1, ERR);
	}
	/* init of global vars */
	loutput = false;
	outmut = &mutex_out;
	arrmut = &mutex_arr;
	empty = &empty_cond;
	pthread_mutex_init(outmut, NULL);
	pthread_mutex_init(arrmut, NULL);
	pthread_cond_init(empty, NULL);
	ind = -1;
	end = false;
	names = (char **) malloc(sizeof (char *));

	/* parsing arguments except last two */
	optc = argc - 2;
	char *optv[optc], *dirname;

	/* last 2 args are PATTERN and DIR */
	needle = argv[argc - 2];
	dirname = argv[argc - 1];

	for (i = 0; i < optc; ++i)
		optv[i] = argv[i];

	while ((c = getopt(optc, optv, "mlho:")) != -1) {
		switch (c) {
			case 'm':
				mapon = true;
				break;
			case 'l':
				loutput = true;
				break;
			case 'h':
				print_usage(argv[0]);
				break;
			case 'o':
				infile = optarg;
				break;
			default:
				errx(1, ERR);
			}
	}

	if (infile) {
		if (!(fout = fopen(infile, "w"))) {
			perror(infile);
			exit(1);
		}
	}
	else
		fout = stdout;

	if (mapon) {
		int len = strlen(needle);
		zpet = (int *) malloc(sizeof(int)*len);
		zpet = hrany(needle, zpet, len);
	}

	/* checking if input dirname is not an file */
	if (stat(dirname, &stats) < 0)
			errx(2, "problem with stat on file %s\n", dirname);

	if (!S_ISDIR(stats.st_mode)) {
		if (mapon)
			msearch(dirname);
		else
			search(dirname);
		return (0);
	}

	/* creating worker threads */
	for (i = 0; i < NUM_THREADS; ++i) {
		if ((erro =
			(pthread_create(&thread[i], NULL, thr_run, NULL)) != 0))
		    errx(1, "pthread_create: %s", strerror(erro));
	}
	// fprintf(stderr, "threads created\n");

	explor(dirname, true);


	for (i = 0; i < NUM_THREADS; ++i) {
		pthread_join(thread[i], NULL);
	}
	// fprintf(stderr, "all done\n");
	free(zpet);
	return (0);
}

void print_usage(char *prog) {
	fprintf(stderr, "usage: %s [-lm] [-o OUTFILE] PATTERN DIR\n", prog);
	fprintf(stderr, "parameter -l turns on context output\n");
	fprintf(stderr, "parameter -m forces using of mmap\n");
	fprintf(stderr, "-o OUTFILE specifies the output file,");
	fprintf(stderr, " default is stdout\n");
}

int *hrany(char *pattern, int *out, int len) {
	int n = 0;
	int i;

	out[0] = 0; // out[0] is always 0
	i = 1;

	// the loop calculates out[i] for i = 1 to len-1
	while (i < len) {
		if (pattern[i] == pattern[n]) {
			n++;
			out[i] = n;
			i++;
		} else /* (pattern[i] != pattern[n]) */ {
			if (n != 0)
				n = out[n-1];
			else {
				out[i] = 0;
				i++;
			}
		}
	}
	return (out);
}