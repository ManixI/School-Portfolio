#ifndef WORDPAIRS_H
#define WORDPAIRS_H

// gets array of file names from cmd line
int main (int argc, char *argv[]);

// gets data from table in linked list, reads it into an array, 
// sorts array by word pair frequency, then prints entire array
// returns 3 if memory failed to allocate
// else returns 0
int printData(struct hashTable *table, int count);

#endif
