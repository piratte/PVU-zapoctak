#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <err.h>
#include <memory.h>

#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "explor.h"
#include "main.h"


int
main(int argc, char *argv[])
{
	int i, optc, erro;
	char c = ' ';

	if (argc < 2)
			errx(1, "usage: %s [-l] PATTERN DIR", argv[0]);

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

	while ((c = getopt(optc, optv, "l")) != -1) {
	switch (c) {
		case 'l':
			loutput = true;
			break;
		default:
			fprintf(stderr, "Incorect param., use -l -1\n");
			abort();
		}
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

	return (0);
}