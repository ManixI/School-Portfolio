/***********************************************************************
name:
	readable -- recursively count readable files.
description:	
	See CS 360 Files and Directories lectures for details.
***********************************************************************/

/* Includes and definitions */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/limits.h>

/**********************************************************************
Take as input some path. Recursively count the files that are readable.
If a file is not readable, do not count it. Return the negative value
of any error codes you may get. However, consider what errors you want
to respond to.
**********************************************************************/

/* Error Codes:
	* -2  file/directory dose not exist
	* -13 no access to inital directory */


// TODO: error handling
// TODO: if dosn't work use multiple functions

int readable(char* inputPath) {
	char *directory = calloc(PATH_MAX+1, sizeof(char));
	int tmp;
	int count = 0;
	char *fileP;

	// if no path is provided us working directory
	if(inputPath==NULL) {
		directory = getcwd(directory, PATH_MAX);
	} else {
		// else copy to directory to use same variable
		strcpy(directory, inputPath);
	}

	// file dose not exist
	if (access(directory, F_OK) != 0) {
		free(directory);
		return -1 * errno;
	}

	// if file/dir can't be read return permision error
	int acs = access(directory, R_OK);
	if (acs == -1) {
		if (errno == EACCES) {
			free(directory);
			return -1 * EACCES;
		}
	}

	// open dir
	DIR *dir = opendir(directory);
	// if not a dir, but was redable, it was a redable file, count 1
	if (dir == NULL) {
		if (errno == ENOTDIR) {
			free(directory);
			return 1;
		}
	}

	// get first object in directory
	struct dirent *ent = readdir(dir);

	// call readable on every file in directory
	while (ent != NULL) {
		if (strcmp(ent->d_name, "..") == 0 || strcmp(ent->d_name, ".") == 0 ) {
			ent = readdir(dir);
			continue;
		}
		// assemble path for current file
		fileP = calloc(PATH_MAX+1, sizeof(char));
		strcpy(fileP, directory);
		strcat(fileP, "/");
		strcat(fileP, ent->d_name);

		// recursivly loop through all entries
		tmp = readable(fileP);
		if (tmp > 0) {
			count = count + tmp;
		}
		
		free(fileP);
		ent = readdir(dir);
	}
	// cleanup
	closedir(dir);
	free(directory);
	return count;
}