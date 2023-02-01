#include "hashTable.h"
#include <stdio.h>
#include <stdlib.h>

#define TEST_1 0
#define TEST_2 0
#define TEST_3 0
#define TEST_4 0
#define TEST_5 0
#define TEST_6 0
#define TEST_7 0
#define TEST_8 1

int main() {
	printf("Running Tests...\n");

// test table mallocs correctly
#if TEST_1
	printf("----- Test 1 -----\n");
	struct  hashTable *table = newTable(5);
	printf("table size: %i\n", table->size);
	free(table->tbl);
	free(table);
	printf("------------------\n\n");
#endif

// test wipe delets empty table
#if TEST_2
	printf("----- Test 2 -----\n");
	struct hashTable *table = newTable(5);
	wipe(table);
	printf("------------------\n\n");
#endif

// tests insert works correctly
// tests wike clears entries properly
#if TEST_3
	printf("----- Test 3 -----\n");
	struct hashTable *table = newTable(5);
	table = insert("test", table);
	table = insert("test", table);
	table = insert("hello world", table);
	//struct bucket *tmp;
	printTable(table);
	wipe(table);
	printf("------------------\n\n");
#endif

// test table grows properly
#if TEST_4
	printf("----- Test 4 -----\n");
	struct hashTable *table = newTable(1);
	printf("inital table size is: %i\n", table->size);
	table = insert("one", table);
	table = insert("two", table);
	table = insert("three", table);
	table = insert("four", table);
	table = insert("five", table);
	printTable(table);
	wipe(table);
	printf("------------------\n\n");
#endif

// test grow mantains word count properly
#if TEST_5
	printf("----- Test 5 -----\n");
	struct hashTable *table = newTable(1);
	printf("inital table size is: %i\n", table->size);
	table = insert("one", table);
	table = insert("two", table);
	table = insert("three", table);
	table = insert("three", table);
	table = insert("three", table);
	table = insert("three", table);
	table = insert("four", table);
	table = insert("five", table);
	printTable(table);
	wipe(table);
	printf("------------------\n\n");
#endif

// test compare function
#if TEST_6
	printf("----- Test 6 -----\n");
	struct hashTable *table = newTable(1);
	table = insert("one", table);
	table = insert("two", table);
	table = insert("three", table);
	table = insert("three", table);
	table = insert("three", table);
	table = insert("three", table);
	struct bucket *one = table->tbl[0];
	struct bucket *two = table->tbl[0]->next;
	struct bucket *three = table->tbl[0]->next->next;
	printf("bucket one: %i %s\n", one->count, one->str);
	printf("bucket two: %i %s\n", two->count, two->str);
	printf("bucket three: %i %s\n", three->count, three->str);
	struct bucket *tmp = cmpBucket(one, two);
	printf("comparing one and two returns: %s\n", tmp->str);
	tmp = cmpBucket(two, one);
	printf("comparing two and one returns: %s\n", tmp->str);
	tmp = cmpBucket(one, three);
	printf("comparing one and three reutrns: %s\n", tmp->str);
	tmp = cmpBucket(three, one);
	printf("comparing three and one reutrns: %s\n", tmp->str);
	printf("------------------\n");
	printTable(table);
	wipe(table);
	printf("------------------\n\n");
#endif

// test grow, seems to break after second grow
#if TEST_7
	printf("----- Test 7-----\n");
	struct hashTable *table = newTable(1);
	printf("inital table size is: %i\n", table->size);
	table = insert("one", table);
	table = insert("two", table);
	table = insert("three", table);
	table = insert("three", table);
	table = insert("three", table);
	table = insert("three", table);
	table = insert("four", table);
	table = insert("five", table);
	printTable(table);
	table = grow(table, table->size*3);
	printTable(table);
	table = grow(table, table->size*3);
	printTable(table);
	wipe(table);
	printf("------------------\n\n");
#endif

// tests getData and freeData functionmak
#if TEST_8
	printf("----- Test 8-----\n");
	struct hashTable *table = newTable(1);
	printf("inital table size is: %i\n", table->size);
	table = insert("one", table);
	table = insert("two", table);
	table = insert("three", table);
	table = insert("three", table);
	table = insert("three", table);
	table = insert("three", table);
	table = insert("four", table);
	table = insert("five", table);
	printTable(table);
	printf("----- getData -----\n");
	struct bucket *list = getData(table);
	struct bucket *cursor = list;
	while (cursor!=NULL) {
		printf("line: '%s', count: %i\n", cursor->str, cursor->count);
		cursor = cursor->next;
	}
	printf("----- Did the table change? -----\n");
	printTable(table);
	printf("----- How about after freeing the list?\n");
	freeData(list);
	printTable(table);
	wipe(table);
	printf("------------------\n\n");
#endif

	return 0; 
}
