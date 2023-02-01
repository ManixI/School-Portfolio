#include <stdio.h>
#include <stdlib.h>
#include "lineNum.h"

int main(void) {
    int num;
    
    char *word = "dog";
    num = lineNum("webster_16", word, 16);
    printf("%s: %i\n",word, num);
    printf("-----------------\n");

    char *word1 = "aardvark";
    num = lineNum("webster_16", word1, 16);
    printf("%s: %i\n",word1, num);
    printf("-----------------\n");

    char *word2 = "qrs";
    num = lineNum("webster_16", word2, 16);
    printf("%s: %i\n",word2, num);
    printf("-----------------\n");
/*
    char *word3 = "fi sh ";
    num = lineNum("tiny_9", word3, 9);
    printf("%s: %i\n",word3, num);
    printf("-----------------\n");

    char *word4 = "hob";
    num = lineNum("tiny_9", word4, 9);
    printf("%s: %i\n",word4, num);
    printf("-----------------\n");

    char *word5 = "aaardvark";
    num = lineNum("tiny_9", word5, 9);
    printf("%s: %i\n",word5, num);
    printf("-----------------\n");

    char *word6 = "nellow";
    num = lineNum("tiny_9", word6, 9);
    printf("%s: %i\n",word6, num);
    printf("-----------------\n");
*/


    return 0;
}
