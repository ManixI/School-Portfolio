#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[]) {
    // check if passed enough arguments
    /*if (argc < 2) {
        printf("my-cat takes at least one argument.\n");
        printf("Example \"./my-cat /usr/docs/foo.txt\"\n");
        return -1;
    }*/

    FILE *fp;

    // loop through all files passed in
    for (int i=1; i<argc; i++) {
        fp = fopen(argv[i], "r");

        if (fp == NULL) {
            printf("my-cat: cannot open file\n");
            //printf("could not open file %s\n", argv[i]);
            //perror("Error: ");
            // don't end because only some of multiple files could be bad
            //continue;
            return 1;
        }

        // print file
        char buffer[255];
        while (fgets(buffer, 255, fp) != NULL) {
            printf("%s", buffer);
        }

        fclose(fp);
    }
    
    return 0;
}