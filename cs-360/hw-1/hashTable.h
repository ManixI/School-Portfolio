#ifndef HASHTABLE_H
#define HASHTABLE_H

#define TRUE_T 1
#define FALSE_T 0

// Hasihng is done with crc_64
// to find the hash of a specific string for a given table use:
// crc64(*string) % hashTable->size


// bucket for each spot in hash table
// uses linked list in case of collision

struct bucket {
	char *str;
	int count;
	struct bucket *next;
};

// hash tbale structure
// array of buckets that also can hold it's size
// used over simple array so to better track table size
// grow safe used to avoid calling grow mid grow
// 0 = safe, 1 = unsafe, 2 = unsafe and needs to grow again
// numBuckets is the number of seperate entries in the table
struct hashTable {
	int size;
	int growSafe;
	int numBuckets;
	struct bucket **tbl;
};

// return a new table with spaces equale to size
struct hashTable* newTable (int size);

// inserts string into table, if string already exists increment it's
// count instead
// return pointer to hash table as it will grow the table if there are 
// more then 4 objects in the same bucket
struct hashTable* insert (char *string, struct hashTable *table);

// grow the given table to newSize, copeying all data from old
// table then freeing it
// returns new hashTable
// Negative newSize is converted to positive equivilent
// newSize of 0 returns NULL
struct hashTable* grow(struct hashTable *table, int newSize);

// helper function for the grow function
// walks a list and inserts every item on it into the given table
void lineCopy (struct hashTable *dst, struct bucket *src, int newSize);

// used to free all memory for a given table
void wipe(struct hashTable *table);

// helper function for wipe
// frees all memory for given list of buckets
void wipeLine(struct bucket *line);

// return data in hashtable as single linked list
// return data in hash table in linked list form
// walk down array, appending the first buket in any 
// used so you don't need to dig though table yourself
struct bucket* getData(struct hashTable *table);

// call this to free memory allocated in getData
// frees a linked list of buckts recursivly
// dose the same thing as wipeLine
void freeData(struct bucket *list);

// prints hash table
// for debuging, not for final output
// output not meangfully sorted
void printTable(struct hashTable *table);

// prints everything in table in bucket(index)
// for debuging purposes
void printBucket(struct hashTable *table, int bucket);

// comares the count of bucket one and two
// returns p1->count - p2->count
// compare function usable by qsort()
int cmpBucketP(const void *p1, const void *p2);

#endif
