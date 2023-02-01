#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashTable.h"
#include "getWord.h"
#include "wordPairs.h"
#include "crc64.h"

// run with hardcoded file
// set to 1 if using gdb as it wont recogonise file called from cmd line
#define DEBUG 0


int main (int argc, char *argv[]) {
	if (argc == 0) {
		fprintf(stderr, "no file was given\n");
		return 2;
	}
	struct hashTable *table = newTable(27);
	FILE *fp;
	char *wordOne;	
	char *wordTwo;
	char *space = " ";
	char *word;
	int count = 0;

	// check if count is provided and if so set it
	if (atoi(argv[1])) {
		count = atoi(argv[1]);
		if (count < 1) {
			count = 0;
		}
	}

	// change first argv to open if count is provided
	int j = 1;
	if (count > 0) {
		j = 2;
	}

#if !DEBUG
	// loop though files given, inserting the wordpairs into the table
	for (int i = j; i < argc; i++) {
		fp = fopen(argv[i], "r");
		// if file can't be opened, quit with error code 1
		if (!fp){
			fprintf(stderr, "can't open file %s.\nDid you enter a number of lines less than 1?\n", argv[i]);
			return 1;
		}
		wordOne = getNextWord(fp);
		//printf("%s\n", wordOne);
			if (wordOne == NULL) {
				fprintf(stderr, "file %s was emtpy\n", argv[i]);
				break;
			}
		// add each word pair in argv[i] to table
		while (1) {
			wordTwo = getNextWord(fp);
			if (wordTwo == NULL) {
				break;
			}
			//printf("%s\n", wordTwo);
			// may not need malloc here as in theory the strcpy in insert will only ready
			// to first \0 and ignore everything beyond
			word = (char *) calloc((strlen(wordOne)+strlen(wordTwo)+3), sizeof(char));
			strncat(word, wordOne, (strlen(wordOne) + 1) * sizeof(char));
			strcat(word, space);
			strcat(word, wordTwo);

			strcpy(wordOne, wordTwo);
			table = insert(word, table);
			free (wordTwo);
			free(word);
		}
		fclose(fp);
	}
	free(wordOne);
	free(wordTwo);
#endif

// debuge code with file hard coded
#if DEBUG 
	printf("----- DEBUG MODE ------\n");
	char *file = "gettysburg.txt";
	fp = fopen(file, "r");
		// if file can't be opened, quit with error code 1
		if (!fp){
			fprintf(stderr, "can't open file %s\n", file);
			return 1;
		}
		wordOne = getNextWord(fp);
		//printf("%s\n", wordOne);
			if (wordOne == NULL) {
				fprintf(stderr, "file %s was emtpy\n", file);
				//break;
			}
		// add each word pair in argv[i] to table
		while (1) {
			wordTwo = getNextWord(fp);
			if (wordTwo == NULL) {
				break;
				//printf("someting went wrong\n");
			}
			//printf("%s\n", wordTwo);
			// may not need malloc here as in theory the strcpy in insert will only ready
			// to first \0 and ignore everything beyond
			word = (char *) calloc((strlen(wordOne)+strlen(wordTwo)+3), sizeof(char));
			strcat(word, wordOne);
			strcat(word, space);
			strcat(word, wordTwo);
			//printf("%s %li\n", word, strlen(word));

			strcpy(wordOne, wordTwo);
			table = insert(word, table);
			free(wordTwo);
			free(word);
		}
	free(wordOne);
	free(wordTwo);
	//printTable(table);
#endif

	int err = printData(table, count);
	if (err != 0) {
		return err;
	}
	wipe(table);
	return 0;
}

// gets data from table in linked list, reads it into an array, 
// sorts array by word pair frequency, then prints entire array
int printData (struct hashTable *table, int count) {
	struct bucket *list = getData(table);
	if (list == NULL) {
		printf("list function failed to allocate memory");
		return 3;
	}
	struct bucket *cursor = list;
	int length = 0;
	// get num elements in hashtable
	while (cursor) {
		cursor = cursor->next;
		length++;
	}
	// put elements of hash table into array
	struct bucket *arr = calloc(length, sizeof(struct bucket));
	if (arr == NULL) {
		printf("memory allocation error\n");
		return 3;
	}
	cursor = list;
	for (int i = 0; i < length; i++) {
		arr[i] = *cursor;
		cursor = cursor->next;
	}
	// sort table
	qsort(arr, length, sizeof(struct bucket), cmpBucketP);
	// if count != 0 only print first count lines
	if ((count > 0) && count < length){
		length = count;
	}
	for (int i = 0; i < length; i++) {
		printf("%10d %s\n", arr[i].count, arr[i].str);
	}
	freeData(list);
	free(arr);
	return 0;
}


