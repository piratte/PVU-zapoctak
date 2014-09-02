#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h> 

#include <errno.h>
#include <err.h>
#include <memory.h>

#include "explor.h"

char *needle;
bool loutput;


// bool search(char *filename) {
// 	printf("searching: %s\n", filename);
// 	return (true);
// }h


// int explor(char *dirname) {
// 	DIR *d;
// 	struct dirent *de;
// 	char *name;
// 	struct stat stats;

// 	if ((d = opendir(dirname)) == NULL) {
// 		warn("%s", dirname);
// 		return (-1); 
// 	}

// 	printf("== [%s] ==\n", dirname);

// 	errno = 0;
// 	while ((de = readdir(d)) != NULL) {
// 		/* filtering out dirs . and .. */
// 		if (de->d_name[0] == '.') 
// 			continue;
// 		/* composing filename */
// 		name = (char *) malloc(2 + strlen(dirname) + strlen(de->d_name));
// 		strcpy(name, dirname);
// 		strcat(name, lom);
// 		strcat(name, de->d_name);
// 		/* figuring out, whether file or dir*/
// 		if (stat(name, &stats) < 0)
// 				errx(2, "problem with stat on file %s\n", name);
				
//       /* files are searched, dirs are explored */
// 		S_ISDIR(stats.st_mode) ? explor(name) : search(name) ;

// 		if (errno != 0)
// 				warn("%s", name);
// 	}
// 	closedir(d);
// 	return (true);
// }


int main(int argc, char *argv[])
{
	int i, optc;
	char c = ' ';

	if (argc < 2)
			errx(1, "usage: %s [-l] PATTERN DIR", argv[0]);

	/* parsing arguments except last two */
	optc = argc - 2;

	char *optv[optc], *dirname;

	
	loutput = false;
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

	explor(dirname, needle, loutput);
	return (0);
}