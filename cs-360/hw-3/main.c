#include <stdio.h>
#include <stdlib.h>
#include "assignment3.h"

int main (void) {
    int count = 0;

    count = readable(NULL);
    printf("count in working directory is %i\n", count);

    count = readable("/home/ian/dev/cs-360/hw-3/Test");
    printf("count in Test directory is %i\n", count);

    count = readable("Test");
    printf("count in Test directory is %i\n", count);

    count = readable("Test/t.txt");
    printf("count in Test/t.txt directory is %i\n", count);

    count = readable("/home/ian/dev/cs-360/hw-3/test-2");
    printf("count in test-2 directory is %i\n", count);

    count = readable("/home/ian/dev/cs-360/hw-3/test-3");
    printf("count in test-3 directory is %i\n", count);

    return 0;
}