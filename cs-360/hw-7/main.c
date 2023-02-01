#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "assignment7.h"


int compare (const void *a, const void *b ) {
    const char *pa = *(const char**)a;
    const char *pb = *(const char**)b;

    return strcmp(pa,pb);
}

//-------------------------------------FIND TOTAL NUMBER OF WORDS--------------------------------

int main(int argc, char **argv) {
	const char *filename = "poem.txt";
	char c, str[20];
	int wordCount = 0, i = 0;
	clock_t start, end;
	double cpuTimeUsed, cpuTimeUsed2;
	
	if(argc != 2) {
		fprintf(stderr, "./sort numberOfThreads\n");
		exit(1);
	}
	int numT = atoi(argv[1]);	
	setSortThreads(numT);  //Set the number of threads to be used
	printf("Number of threads to be used: %d\n",numT);

	FILE *fp = fopen(filename, "r");
	if(fp == NULL) {
		fprintf(stderr,"file open error\n");
		exit(1);
	}

	//Count the number of words in the file
	while( (c = fgetc(fp)) != EOF) {
		if(c == ' ' || c == '\n' || c == '.' || c == ':' || c == '' || c == ',' || c == '[' || c == ']' || c == ';' || c == '?' || c == '!') {
			str[i] = '\0';
			if(str[0] == '\0') {
				continue;
			}
			else {
				++wordCount;
				//printf("%s %d\n",str,wordCount);
				memset(str, 0, sizeof(str));
				i = 0;
			}
		}
		else {
			str[i] = c;
			i++;
		}
	}
	fclose(fp);

//--------------------------------MALLOC CHAR **ARRAY AND INSERT WORDS----------------------------
//
	//Malloc a char ** array the size of the total wordCount for quicksort
	char **wordArray = (char **) malloc(wordCount * sizeof(char *));
	for(int i = 0; i < wordCount; i++) {
		//Each char * (word) in the array can be a maximum of 20 char in length
		wordArray[i] = (char *) malloc(20 * sizeof(char));
	}

	//Malloc a char ** array2 the size of the total wordCount for qsort()
	char **wordArray2 = (char **) malloc(wordCount * sizeof(char *));
        for(int i = 0; i < wordCount; i++) {
                //Each char * (word) in the array can be a maximum of 20 char in length
                wordArray2[i] = (char *) malloc(20 * sizeof(char));
        }

	i = 0;  //Keep track of characters in single word
	int j = 0; //Keep track of where to insert into **wordArray

	fp = fopen(filename, "r");
	if(fp == NULL) {
                fprintf(stderr,"file open error\n");
                exit(1);
        }

	while( (c = fgetc(fp)) != EOF) {
        	if(c == ' ' || c == '\n' || c == '.' || c == ':' || c == '' || c == ',' || c == '[' || c == ']' || c == ';' || c == '?' || c == '!') {                
			str[i] = '\0';
                        if(str[0] == '\0') {
                                continue;
                        }
                        else {
				for(int k = 0; k < sizeof(str); k++) { //sizeof to include \0
					wordArray[j][k] = str[k];
					wordArray2[j][k] = str[k];
				}
				j++;
                                memset(str, 0, sizeof(str));
				str[0] == '\0';
                                i = 0;
                        }
                }
                else {
                        str[i] = c;
                        i++;
                }
        }
	fclose(fp);

	//printf("There are a total of %d words\n",wordCount);
	//sleep(5);
	//for(int i = 0; i < wordCount; i++) {
	//	printf("%s --- inserted into wordArray[%d]\n",wordArray[i],i);
	//}	
	//sleep(5);
//-------------------------------------TIME TO SORT-------------------------------------
	start = clock();
	//printf("%s\n", wordArray[1]);
	qsort(wordArray,wordCount,sizeof(char *),compare);
	end = clock();
	cpuTimeUsed = ((double) (end - start)) / CLOCKS_PER_SEC;
	
	start = clock();
	sortThreaded(wordArray2, wordCount);
	end = clock();
	cpuTimeUsed2 = ((double) (end - start)) / CLOCKS_PER_SEC;

	//sleep(5);
	//for(int i = 0; i < wordCount; i++) {
        //        printf("%s --- sorted wordArray[%d]\n",wordArray[i],i);
        //}
	//sleep(5);
	//for(int i = 0; i < wordCount; i++) {
        //        printf("%s --- sorted wordArray2[%d]\n",wordArray2[i],i);
        //}

	printf("Benchmark qsort: %f\n",cpuTimeUsed);
	printf("Optimized sortThreaded quicksort: %f\n",cpuTimeUsed2);

	//free memory for char** array
	for(int i = 0; i < wordCount; i++) {
		free(wordArray[i]);
	}
	free(wordArray);

	//free memory for char** array2
        for(int i = 0; i < wordCount; i++) {
                free(wordArray2[i]);
        }
        free(wordArray2);
}
