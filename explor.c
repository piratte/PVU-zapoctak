#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>
#include <err.h>
#include <memory.h>

#include "explor.h"
#include "main.h"
#include "search.h"

int
explor(char *dirname, bool top) {
	char *lom = "/";
	DIR *d;
	struct dirent *de;
	char *name;
	struct stat stats;

	if ((d = opendir(dirname)) == NULL) {
		warn("%s", dirname);
		exit(-1);
	}

	/* info print */
	// printf("== [%s] ==\n", dirname);

	errno = 0;
	while ((de = readdir(d)) != NULL) {
		/* filtering out dirs . and .. */
		if (de->d_name[0] == '.')
			continue;
		/* composing filename */
		name = (char *)
			malloc(2 + strlen(dirname) + strlen(de->d_name));
		strcpy(name, dirname);
		strcat(name, lom);
		strcat(name, de->d_name);
		/* figuring out, whether file or dir */
		if (stat(name, &stats) < 0)
				errx(2, "problem with stat on file %s\n", name);
		/* files are searched, dirs are explored */
		if (S_ISDIR(stats.st_mode))
			explor(name, false);
		else {
			pthread_mutex_lock(arrmut);
			add(name);
			pthread_cond_broadcast(empty);
			pthread_mutex_unlock(arrmut);
		}

		if (errno != 0)
				warn("%s", name);
	}
	if (top) {
		end = true;
		pthread_cond_broadcast(empty);
	}
	closedir(d);
	return (true);
}

void *
thr_run(void *x) {
	// fprintf(stderr, "thread: running\n");


	pthread_mutex_lock(arrmut);
	/* no more producing and work is done */
	while (!(end && (ind == -1))) {
		/*
		 * wait untill there is something to search,
		 * or there will be nothing more
		 */
		while ((ind < 0) && !end)
			pthread_cond_wait(empty, arrmut);

		if (ind < 0) {
			pthread_mutex_unlock(arrmut);
			break;
		}

		// fprintf(stderr, "thread %d analyzing: ind = %d\n", pthread_self(),ind - 1);
		char *src = get_src();
		pthread_mutex_unlock(arrmut);
		search(src);
		free(src);
		pthread_mutex_lock(arrmut);
	}
	pthread_mutex_unlock(arrmut);
	return (NULL);
}

void
add(char *name) {
	// fprintf(stderr, "adding: %s, index = %d\n", name, ind);
	++ind;
	names = (char **) realloc(names, (ind+1)*(sizeof (char *)));
	names[ind] = name;
	// fprintf(stderr, "added: %s\n", name);
}

/* mutex is in the outer function */
char *
get_src() {
	/* getting filename for analyzing */

	char *result = names[ind];
	--ind;
	return (result);
}