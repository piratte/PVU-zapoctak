#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdbool.h>

#include <errno.h>
#include <err.h>
#include <memory.h>

#include "explor.h"

int explor(char *dirname, char *needle) {
	char *lom = "/"; 
	DIR *d;
	struct dirent *de;
	char *name;
	struct stat stats;

	if ((d = opendir(dirname)) == NULL) {
		warn("%s", dirname);
		return (-1); 
	}

	printf("== [%s] ==\n", dirname);

	errno = 0;
	while ((de = readdir(d)) != NULL) {
		/* filtering out dirs . and .. */
		if (de->d_name[0] == '.') 
			continue;
		/* composing filename */
		name = (char *) malloc(2 + strlen(dirname) + strlen(de->d_name));
		strcpy(name, dirname);
		strcat(name, lom);
		strcat(name, de->d_name);
		/* figuring out, whether file or dir*/
		if (stat(name, &stats) < 0)
				errx(2, "problem with stat on file %s\n", name);
				
      /* files are searched, dirs are explored */
		S_ISDIR(stats.st_mode) ? explor(name, needle) : search(name, needle);

		if (errno != 0)
				warn("%s", name);
	}
	closedir(d);
	return (true);
}