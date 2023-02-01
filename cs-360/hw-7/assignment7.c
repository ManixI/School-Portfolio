#include "assignment7.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>

#define SORT_THRESHOLD      40
#define THREAD_THRESHOLD    80

#define DEBUG 0
#define DEBUG2 0

typedef struct _sortParams {
    char** array;
    int left;
    int right;
} SortParams;

static int maximumThreads;              /* maximum # of threads to be used */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int threadcount;


/* This is an implementation of insert sort, which although it is */
/* n-squared, is faster at sorting short lists than quick sort,   */
/* due to its lack of recursive procedure call overhead.          */

static void insertSort(char** array, int left, int right) {
    int i, j;
    for (i = left + 1; i <= right; i++) {
        char* pivot = array[i];
        j = i - 1;
        while (j >= left && (strcmp(array[j],pivot) > 0)) {
            array[j + 1] = array[j];
            j--;
        }
        array[j + 1] = pivot;
    }
}

/* Recursive quick sort, but with a provision to use */
/* insert sort when the range gets small.            */

static void quickSort(void* p) {
    SortParams* params = (SortParams*) p;
    char** array = params->array;
    int left = params->left;
    int right = params->right;
    int i = left, j = right;
    
    pthread_t right_t, left_t;
	int r = 0;
	int l = 0;
    
    if (j - i > SORT_THRESHOLD) {           /* if the sort range is substantial, use quick sort */

        int m = (i + j) >> 1;               /* pick pivot as median of         */
        char* temp, *pivot;                 /* first, last and middle elements */
        if (strcmp(array[i],array[m]) > 0) {
            temp = array[i]; array[i] = array[m]; array[m] = temp;
        }
        if (strcmp(array[m],array[j]) > 0) {
            temp = array[m]; array[m] = array[j]; array[j] = temp;
            if (strcmp(array[i],array[m]) > 0) {
                temp = array[i]; array[i] = array[m]; array[m] = temp;
            }
        }
        pivot = array[m];

        for (;;) {
            while (strcmp(array[i],pivot) < 0) i++; /* move i down to first element greater than or equal to pivot */
            while (strcmp(array[j],pivot) > 0) j--; /* move j up to first element less than or equal to pivot      */
            if (i < j) {
                char* temp = array[i];      /* if i and j have not passed each other */
                array[i++] = array[j];      /* swap their respective elements and    */
                array[j--] = temp;          /* advance both i and j                  */
            } else if (i == j) {
                i++; j--;
            } else break;                   /* if i > j, this partitioning is done  */
        }
        
        SortParams first;  first.array = array; first.left = left; first.right = j;
        //quickSort(&first);                  /* sort the left partition  */
        
        SortParams second; second.array = array; second.left = i; second.right = right;
        //quickSort(&second);	/* sort the right partition */

	// TODO: threading
	// setup for next recursive call
	

        // while threads are avaliable, make thread for each recursion
	    // make right thread
        
        if (threadcount < maximumThreads) {
        	#if DEBUG2
        		printf("makeing right thread\n");
        	#endif
                if (pthread_mutex_lock(&mutex)) {
                        perror("lock error: ");
                }
                if (pthread_create(&right_t, NULL, (void *)quickSort, &first)) {
                        perror("thread create error: ");
                }
                threadcount++;
                r = 1;
                if (pthread_mutex_unlock(&mutex)) {
                        perror("unlock error: ");
                }
                #if DEBUG
               	printf("%i of %i threads in use\n",threadcount, maximumThreads);
       	 #endif
        } else {
                quickSort(&first);
        }

        // make left thread
        
	
        if (threadcount < maximumThreads) {
        	#if DEBUG2
        		printf("makeing left thread\n");
        	#endif
                if (pthread_mutex_lock(&mutex)) {
                        perror("lock error: ");
                }
                if (pthread_create(&left_t, NULL, (void *)quickSort, &second)) {
                        perror("thread create error: ");
                }
                threadcount++;
                l = 1;
                if (pthread_mutex_unlock(&mutex)) {
                        perror("unlock error: ");
                }
                #if DEBUG
                	printf("%i of %i threads in use\n",threadcount, maximumThreads);
        	#endif
        } else {
                quickSort(&second);
        }

	
        if (r == 1) {
		#if DEBUG
			printf("waiting on right thread\n");
		#endif
                if (pthread_join(right_t, NULL)) {
                        perror("join error: ");
                }
        }
        
        if (l == 1) {
		#if DEBUG
		        printf("waiting on left thread\n");
		#endif
                if (pthread_join(left_t, NULL)) {
                        perror("join error: ");
                }
    	} 
    } else {
    	insertSort(array,i,j);
    }           /* for a small range use insert sort */
}

/* user interface routine to set the number of threads sortT is permitted to use */

void setSortThreads(int count) {
    maximumThreads = count;
}

/* user callable sort procedure, sorts array of count strings, beginning at address array */

void sortThreaded(char** array, unsigned int count) {
    SortParams parameters;
    parameters.array = array; parameters.left = 0; parameters.right = count - 1;
    quickSort(&parameters);
}
