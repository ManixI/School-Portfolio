#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>


char* replaceInLine(char *find, char *replace, char *line) {
    if (strlen(find) > strlen(line)) {
        return;
    }

    char *buffer = malloc(sizeof(char) * strlen(find)+1);
    char *out = malloc(sizeof(char) * (strlen(line)*2));
    buffer[strlen(find)] = '\0';
    for (int i=0; i<strlen(line) - strlen(find); i++) {
        copyStr(buffer, line, i);
        if (strcmp(find, buffer)) {
            // write new word to out buffer
            for (int j=0; j<strlen(replace); j++) {
                out[i+j] = replace[j];
            }
            out[i+strlen(replace)] = '\0';
            i += strlen(replace);
            if (strlen(out) >= (malloc_usable_size((void*) out) / sizeof(char)) / 2) {
                // if more then half of buffer is used, re-size it
                
            }
        }
    }
}

void copyStr(char *source, char *dest, int startPos) {
    if (strlen(dest) > strlen(source) + startPos) {
        return;
    }

    for (int i=startPos; i<strlen(source)-1; i++) {
        dest[i] = source[i];
    }
    dest[strlen(dest)-1] = '\0';
    return;
}

int main(int argc, char *argv[]) {
 
    if (argc < 3) {
        printf("my-sed: find term replace term [file ...]\n");
        return 1;
    }

    char *find = argv[1];
    char *replace = argv[2];
    if (strcmp(replace, "\"\"")) {
        replace = "";
    }
    int len = strlen(find);

    if (argc == 3) {
        // no file was specified, run on stdin
        while(1) {

        }
        return 0;
    }
 
    FILE *ifp, *ofp;
    char *line = NULL;
    char *locPointer = NULL;
    char word[255] = NULL;
    int linelen = 0;
    int *n = 0;
    for (int i=4; i<argc; i++) {
        // read from files passed as args
        ifp = fopen(argv[i], "r");

        if (ifp == NULL) {
            printf("my-sed: cannot open file\n");
            return 1;
        }
        ofp = fopen(".tmp.tmp", "w+");

        while(line = getline(line, 0, ifp) >= 0) {

        }
        
        /*while (linelen != -1) {
            locPointer = strstr(line, find);
            if (locPointer != NULL) {

            }
        }*/

        /*fscanf(fp, "%s", word);
        while (word != NULL) {
            if (strcmp(word, find)) {

            }
        }*/

    }
    return 0;
}
