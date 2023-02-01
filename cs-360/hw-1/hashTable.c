#include "crc64.h"
#include "hashTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define TESTSIZE 81
#define DEBUG_TABLE FALSE_T
#define PRINT_TABLE FALSE_T


// make new table function
struct hashTable* newTable(int size) {
#if DEBUG_TABLE
	printf("----- In DEBUG_TABLE Mode -----\n");
	getchar();
#endif
	struct hashTable *table = (struct hashTable *) malloc(sizeof(struct hashTable));
	if (table == NULL) {
		printf("memory allocation failed\n");
		return NULL;
	}
	table->tbl = (struct bucket **) calloc(size, sizeof(struct bucket *));
	if (table->tbl == NULL) {
		printf("memory allocation failed\n");
		return NULL;
	}
	table->size = size;
	table->growSafe = 0;
	table->numBuckets = 0;
	return table;
}

// Insert function
struct hashTable* insert(char *string, struct hashTable *table) {
	unsigned long long hash = crc64(string);
	hash = hash % table->size;
	struct bucket *tmp = table->tbl[hash];
	int depth = 0;
	int flag = 0;
	
#if DEBUG_TABLE
	if (table->size == TESTSIZE) {
		printf("Inserting '%s' at index %lli\n", string, hash);
	}
#endif
	
	// if first bucket is empty
	if (!tmp) {
		table->tbl[hash] = (struct bucket *) malloc(sizeof(struct bucket));
		if (table->tbl[hash] == NULL) {
			printf("memory allocation failed\n");
			return NULL;
		}
		tmp = table->tbl[hash];
		tmp->str = (char *) malloc(sizeof(char) * strlen(string)+1);
		if (tmp->str == NULL) {
			printf("memory allocation failed\n");
			return NULL;
		}
		tmp->count = 1;
		strcpy(tmp->str, string);
		tmp->next = NULL;
		table->numBuckets++;
		flag = 1;
	}
	// if first bucket is not empty
	if (!flag) {
		while (tmp) {
			if (!strcmp(string, tmp->str)) {
				tmp->count++;
				flag = 1;
				break;
			}
			depth++;
			// break before temp is set to NULL so as to not lose place
			if (!tmp->next) {
				break;
			}
			tmp = tmp->next;
		}
	}
	// insert at end of list if not in list
	if (!flag) {
		tmp->next = (struct bucket *) malloc(sizeof(struct bucket));
		if (tmp->next == NULL) {
			printf("memory allocation failed\n");
			return NULL;
		}
		tmp->next->str = (char *) malloc(sizeof(char) * strlen(string)+1);
		if (tmp->next->str == NULL) {
			printf("memory allocation failed\n");
			return NULL;
		}
		strcpy(tmp->next->str, string);
		tmp->next->count = 1;
		tmp->next->next = NULL;
		table->numBuckets++;
		depth++;
	}
	// grow table if depth is too high for inserted bucket
	if (depth > 4) {
		if (table->growSafe == 0) {
#if PRINT_TABLE
			printTable(table);
#endif
			table = grow(table, table->size * 3);
		}
		if (table->growSafe == 1) {
			table->growSafe = 2;
		}
	}
	
#if DEBUG_TABLE
	if (table->size == TESTSIZE) {
		printf("Bucket for hash %lli:\n", hash);
		tmp = table->tbl[hash];
		while(tmp) {
			printf("String: %s, Coung: %i, Hash: %lli\n", tmp->str, tmp->count, crc64(tmp->str) % table->size);
			tmp = tmp->next;
		}
		getchar();
	}
#endif
	
	return table;
}

// resize function
struct hashTable* grow(struct hashTable *table, int newSize) {
	
	// error handling for ints < 1
	if (newSize < 0) {
		newSize = 0 - newSize;
	}
	if (!newSize) {
		return NULL;
	}
	// grow table
	struct hashTable *new = newTable(newSize);
	new->growSafe = 1;
	// copy old table to new table
	for (int i = 0; i < table->size; i++){
		if (table->tbl[i] != NULL) {
			lineCopy(new, table->tbl[i], newSize);
		}
	}
	wipe(table);
	if (new->growSafe == 2) {
		new->growSafe = 1;
		new = grow(new, new->size * 3);
	}
	new->growSafe = 1;
	return new;
}

// resize helper function
// copies one list from table recursivly
// src should be a specific bucket
// source is the target table to insert that value into
void lineCopy(struct hashTable *dst, struct bucket *src, int newSize) {
	if (src->next) {
		lineCopy(dst, src->next, newSize);
	}
	for (int i = 0; i < src->count; i++) {
		insert (src->str, dst);
	}
}


// wipe table function
void wipe(struct hashTable *table) {
	// avoides empty table error where tbl is null and thus tbl[i] crashes
	if (table->tbl) {
		for (int i = 0; i < table->size; i++){
			if (table->tbl[i] != NULL) {
				wipeLine(table->tbl[i]);
			}
		}
	}
	free(table->tbl);
	free(table);
}

// helper function for wipe
// recursivly frees lists in each bucket
void wipeLine(struct bucket *line) {
	if (line) {
		wipeLine(line->next);
		free(line->str);
	}
	free(line);
}

// return data in hash table in linked list form
// walk down array, appending the first buket in any 
// hash table list to the list to return
// could maybe also use this to free all data
struct bucket* getData(struct hashTable *table) {
	struct bucket *list = NULL;
	struct bucket *cursor;
	struct bucket *tmp;
	for (int i = 0; i < table->size; i++) {
		if (table->tbl[i] != NULL) {
			// if very first bucket
			if (!list) {
				list = (struct bucket *) malloc(sizeof(struct bucket));
				if (list == NULL) {
					printf("memory allocation error\n");
					return NULL;
				}
				list->str = (char *) malloc(sizeof(char) * (strlen(table->tbl[i]->str) + 1));
				if (list->str == NULL) {
					printf("memory allocation error\n");
					return NULL;
				}
				strcpy(list->str, table->tbl[i]->str);
				list->count = table->tbl[i]->count;
				cursor = list;
				tmp = table->tbl[i];
			// else it's first bucket in list i
			} else {
				tmp = table->tbl[i];
				cursor->next = malloc(sizeof(struct bucket));
				if (cursor->next == NULL) {
					printf("memory allocation error\n");
					return NULL;
				}
				cursor = cursor->next;
				cursor->str = (char *) malloc(sizeof(char) * (strlen(tmp->str) + 1));
				if (cursor->str == NULL) {
					printf("memory allocation error\n");
					return NULL;
				}
				strcpy(cursor->str, tmp->str);
				cursor->count = tmp->count;
			}
			// then walk down list i
			while (tmp->next) {
				tmp = tmp->next;
				cursor->next = malloc(sizeof(struct bucket));
				if (cursor->next == NULL) {
					printf("memory allocation error\n");
					return NULL;
				}
				cursor = cursor->next;
				cursor->str = (char *) malloc(sizeof(char) * (strlen(tmp->str) + 1));
				if (cursor->str == NULL) {
					printf("memory allocation error\n");
					return NULL;
				}
				strcpy(cursor->str, tmp->str);
				cursor->count = tmp->count;
			}
		}
	}
	cursor->next = NULL;
	return list;
}

// call this to free memory allocated in getData
void freeData(struct bucket *list) {
	if (list->next) {
		freeData(list->next);
	}
	free(list->str);
	free(list);
}

// print table
// for debugging only
void printTable(struct hashTable *table) {
	for (int i = 0; i < table->size; i++) {
		struct bucket *tmp = table->tbl[i];
		unsigned long long hash;
		while (tmp != NULL) {
			char *tmpStr = tmp->str;
			hash = crc64(tmpStr);
			hash = hash % table->size;
			printf(" index: %i, hash %lli, string: %s, count: %i\n", i, hash, tmp->str, tmp->count);
			tmp = tmp->next;
		}
	}
}

// print bucket
// for debuging only
void printBucket(struct hashTable *table, int bucket) {
	struct bucket *tmp = table->tbl[bucket];
	while (tmp) {
		printf("String: %s, Coung: %i, Hash: %lli\n", tmp->str, tmp->count, crc64(tmp->str) % table->size);
		tmp = tmp->next;
	}
}

// compare function for qsort
int cmpBucketP(const void *p1, const void *p2) {
	struct bucket *one = (struct bucket *)p1;
	struct bucket *two = (struct bucket *)p2;
	return (two->count - one->count);
}
