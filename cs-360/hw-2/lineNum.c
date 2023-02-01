/*
 * Dictionary Line Search Function
 * This function takes a path to a dictionary [dictionaryName],
 * a word to search for [word], and a the length of each line [dictWidth]
 * This program returns the line number the searched word appears on in
 * the dictionary. If the word is not found it returns the nevative of
 * the last line searched (eg. if the last line was 10, it will return -10).
 * 
 * The dictionary needs to be formatted in the following way:
 *  • Format is 1 word per line
 *  • Lines are in ascending sorted order
 *  • Each line is x characters long
 *  • Use binary search
 *
 * If the dictionary width provided is incorrect the results may be
 * inaccurate. This function will not double check this.
 * On error function will return 0 and set errno:
 * Error codes:
 * ERANGE - word provided is longer then dictWidth
 * ENOENT - the dictionary file cannot be found and/or accessed
 * 3 - the dictionary file is empty
 */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <fcntl.h>


//TODO: test
//TODO: cleanup

int lineNum(char *dictionaryName, char *word, int dictWidth) {
    int i  = 0;
    if (i > dictWidth) {
        perror("Error ERANGE: word bigger then dictWidth ");
        errno = ERANGE;
        return 0;
    }

    char *buffer = (char *) calloc(dictWidth+1, sizeof(char));
    // open file
    int fp = open(dictionaryName, O_RDONLY);
    // if it dosn't exist, return with error
    if (fp == -1) {
        // error message
        perror("Error 2: File not found ");
        return errno;
    }

    // get start and end of file
    long end = lseek(fp, 0, SEEK_END);

    int actual;
    int goUp = 0;
    int notSpace = 0;

    int bottom = 0;
    int top = end / dictWidth;
    int middle = top / 2;

    // used to hold lseek() return value when not used
    int current;

    while(1) {
        // find line to check
        middle = (top + bottom) / 2;
        current = lseek(fp, middle*dictWidth, SEEK_SET);

        // Check if correct line
        actual = read(fp, buffer, dictWidth);
        // error handling for reed()
        if (actual == -1) {
            perror("Error reading file ");
            return 0;
        }  
        // compare word to bufffer, one character at a time
        i = 0;
        while(buffer[i] != '\n' || buffer[i] != '\0') {
            if ((word[i+1] == '\0') && ((buffer[i+1] == '\n') || (buffer[i+1] == '\0'))) {
                break;
            }
            if (word[i] != buffer[i]) {    
                // if the first different character of the word has a lower ascii value 
                // then the corosponding buffer character
                if ((int)word[i] > (int)buffer[i]) {
                    // go up
                    goUp = 1;
                }
                // else go down
                break;
            }
            i++;
        }

        // if match, loop though rest of buffer, make sure it's empty
        notSpace = 0;
        if (word[i] == '\0') {
           while(buffer[i] != '\n' && buffer[i] != '\0') {
                if((buffer[i] != ' ' && word[i] == ' ') && buffer[i]!='\n' && buffer[i]!='\0' ) {
                    notSpace=1;
                    break;
                }
                i++;
            }
        // word contained in larger dict string, not a match
        } else {
            notSpace = 1;
        }

        // if word matches dict entry return line
        if (notSpace == 0) {
            break;
        }

        // if no where else to search, return -1 * line
        if (top == bottom) {
            middle = middle * -1;
            break;
        }
        // deals with rounding issues, also means word not in dictionary
        if (top < bottom) {
            middle = middle * -1;
            break;
        }

        // binary search
        if (goUp == 1) {
            // go left
            goUp = 0;
            bottom  = middle + 1;
        } else {
            // go right
            top = middle - 1;
        }
    // repete until line stops changing or word is found
    }

    close(fp);
    free(buffer);
    return middle;
}
