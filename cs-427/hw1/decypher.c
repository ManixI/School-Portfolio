#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEBUG 1

// ref: https://www3.nd.edu/~busiforc/handouts/cryptography/letterfrequencies.html
// letters in order of frequency they occure in text
const char letterFreq[26] = {'E', 'T', 'A', 'I', 'N', 
                    'O', 'S', 'H', 'R', 'D', 
                    'L', 'U', 'C', 'M', 'F',
                    'W', 'Y', 'G', 'P', 'B',
                    'V', 'K', 'Q', 'J', 'X',
                    'Z'};

// actual values for above
const int letterFreqValue[26] = {1200, 9000, 8000, 8000, 8000, 
                                    8000, 8000, 6400, 6200, 4400,
                                    4000, 3400, 3000, 3000, 2500,
                                    2000, 2000, 1700, 1700, 1600,
                                    1200, 800, 500, 400, 400, 200};

const char letterFreq2[26] = {'E', 'A', 'R', 'I', 'O',
                                'T', 'N', 'S', 'L', 'C',
                                'U', 'D', 'P', 'M', 'H',
                                'G', 'B', 'F', 'Y', 'W',
                                'K', 'V', 'X', 'Z', 'J',
                                'Q'};

// fix this
void replace(char target, char src, char *str, char *out) {
    for (int i=0; i<strlen(str); i++) {
        if (str[i] == target) 
            out[i] = src;
    }
}

int main (int argc, char *argv[]) {
    if (argc < 3) {
        printf("please select mode and message\n");
        return 1;
    }

    if (strcmp(argv[1], "shift") == 0) {
        printf("\n");

        // just print all shifted combinations
        for (int i=0; i<25; i++) {
            for (int j=0; j<strlen(argv[2]); j++) {
                if (argv[2][j] >= 65 && argv[2][j] <= 90) {
                    argv[2][j]++;
                    if (argv[2][j] > 90) 
                        argv[2][j] = 65;
                }
            }
            printf("%s\n\n", argv[2]);
        }

    } else if (strcmp(argv[1], "mono-sub") == 0) {
        // count occurance of each letter in message
        int freq[26];
        for (int i=0; i<26; i++) {
            freq[i] = 0;
        }
        for (int i=0; i<strlen(argv[2]); i++) {
            // convert everything to upper case
            if (argv[2][i] <= 122 && argv[2][i] >=97) {
                argv[2][i] -= 32;
            }

            // count values
            if (argv[2][i] >= 65 && argv[2][i] <=90) {
                // only count alphabet chars
                freq[argv[2][i] - 65]++;
            }
        }

        #if DEBUG 
        for (int i=0; i<26; i++) {
            printf("%d\n", freq[i]);
        }
        #endif

        // sort
        char alphabet[26] = {'A', 'B', 'C', 'D', 'E',
                                'F', 'G', 'H', 'I', 'J',
                                'K', 'L', 'M', 'N', 'O',
                                'P', 'Q', 'R', 'S', 'T',
                                'U', 'V', 'W', 'X', 'Y',
                                'Z'};
        // bubble sort because lazy
        int tmpi;
        char tmpc;
        for (int i=0; i<25; i++) {
            for (int j=i+1; j<26; j++) {
                if (freq[j] > freq[i]) {
                    tmpi = freq[j];
                    tmpc = alphabet[j];
                    freq[j] = freq[i];
                    alphabet[j] = alphabet[i];
                    freq[i] = tmpi;
                    alphabet[i] = tmpc;
                }
            }
        }
        // print freq of each letter
        for (int i=0; i<26; i++) {
            printf("%c %d\n",alphabet[i], freq[i]);
        }

        // count bigrams
        int bigrams[26][26];
        for (int i=0; i<26; i++) {
            for (int j=0; j<26; j++) {
                bigrams[i][j] = 0;
            }
        }

        int x, y;
        for (int i=1; i<strlen(argv[2])-1; i++) {
            x = argv[2][i-1] - 65;
            y = argv[2][i] - 65;
            bigrams[x][y]++;
        }

        #if DEBUG
        for (int i=0; i<26; i++) {
            for (int j=0; j<26; j++) {
                printf("%2i,", bigrams[i][j]);
            }
            printf("\n");
        }
        #endif

        // sort bigrams
        char sortedBigrams[26*26][2];
        int sortedBigramsCount[26*26];
        int max, xMax, yMax;
        // another inefficent sort
        for (int i=0; i<26*26; i++) {
            max = bigrams[0][0];
            xMax = 0;
            yMax = 0;
            for (int j=0; j<26; j++) {
                for (int k=0; k<26; k++) {
                    if (bigrams[j][k]>max) {
                        max = bigrams[j][k];
                        xMax = j;
                        yMax = k;
                    }
                }
            }
            if (max <= 0) {
                // NOTE: do not try and print bigram pairs that have a freq of 0
                for (int j=i; j<26*26; j++) {
                    sortedBigramsCount[j] = 0;
                }
                break;
            }
            sortedBigramsCount[i] = max;
            sortedBigrams[i][0] = (char) (xMax + 65);
            sortedBigrams[i][1] = (char) (yMax + 65);
            bigrams[xMax][yMax] = 0;
        }

        // print bigrams
        for (int i=0; i<26*26; i++) {
            if (sortedBigramsCount[i] <= 3) {
                break;
            }
            printf("%c%c %2d\n", sortedBigrams[i][0], sortedBigrams[i][1], sortedBigramsCount[i]);
        }

        // print estimated message
        char *out = malloc(sizeof(char) * strlen(argv[2]));
        strcpy(out, argv[2]);
        for (int i=0; i<26; i++)  {
            replace(alphabet[i], letterFreq2[i], argv[2], out);
        }
        printf("\n%s\n", out);

        free(out);


    } else if (strcmp(argv[1], "vigen") == 0) {
        // count bigrams
        int bigrams[26][26];
        for (int i=0; i<26; i++) {
            for (int j=0; j<26; j++) {
                bigrams[i][j] = 0;
            }
        }

        int x, y;
        for (int i=1; i<strlen(argv[2])-1; i++) {
            x = argv[2][i-1] - 65;
            y = argv[2][i] - 65;
            bigrams[x][y]++;
        }

        #if DEBUG
        for (int i=0; i<26; i++) {
            for (int j=0; j<26; j++) {
                printf("%2i,", bigrams[i][j]);
            }
            printf("\n");
        }
        #endif

        // sort bigrams
        char sortedBigrams[26*26][2];
        int sortedBigramsCount[26*26];
        int max, xMax, yMax;
        // another inefficent sort
        for (int i=0; i<26*26; i++) {
            max = bigrams[0][0];
            xMax = 0;
            yMax = 0;
            for (int j=0; j<26; j++) {
                for (int k=0; k<26; k++) {
                    if (bigrams[j][k]>max) {
                        max = bigrams[j][k];
                        xMax = j;
                        yMax = k;
                    }
                }
            }
            if (max <= 0) {
                // NOTE: do not try and print bigram pairs that have a freq of 0
                for (int j=i; j<26*26; j++) {
                    sortedBigramsCount[j] = 0;
                }
                break;
            }
            sortedBigramsCount[i] = max;
            sortedBigrams[i][0] = (char) (xMax + 65);
            sortedBigrams[i][1] = (char) (yMax + 65);
            bigrams[xMax][yMax] = 0;
        }

        // print bigrams
        for (int i=0; i<26*26; i++) {
            if (sortedBigramsCount[i] <= 9) {
                break;
            }
            printf("%c%c %2d\n", sortedBigrams[i][0], sortedBigrams[i][1], sortedBigramsCount[i]);
        }


        // count trigrams
        int trigrams[26][26][26];
        for (int i=0; i<26; i++) {
            for (int j=0; j<26; j++) {
                for (int k=0; k<26; k++) {
                    trigrams[i][j][k] = 0;
                }
            }
        }

        //int x, y, z;
        int z;
        for (int i=2; i<strlen(argv[2])-1; i++) {
            x = argv[2][i-2] - 65;
            y = argv[2][i-1] - 65;
            z = argv[2][i] - 65;
            trigrams[x][y][z]++;
        }

        char sortedTrigrams[26*26*26][3];
        int sortedTrigramsCount[26*26*26];
        //int max, xMax, yMax, zMax;
        int zMax;
        for (int i=0; i<26*26*26; i++) {
            xMax = 0;
            yMax = 0;
            zMax = 0;
            max = trigrams[xMax][yMax][zMax];
            for (int x=0; x<26; x++) {
                for (int y=0; y<26; y++) {
                    for (int z=0; z<26; z++) {
                        if (max < trigrams[x][y][z]) {
                            xMax = x;
                            yMax = y;
                            zMax = z;
                            max = trigrams[x][y][z];
                        }
                    }
                }
            }
            if (max <= 0) {
                for (int j=i; j<26*26*26; j++) {
                    sortedBigramsCount[j] = 0;
                }
                break;
            }
            sortedTrigramsCount[i] = max;
            sortedTrigrams[i][0] = (char) (xMax + 65);
            sortedTrigrams[i][1] = (char) (yMax + 65);
            sortedTrigrams[i][2] = (char) (zMax + 65);
            trigrams[xMax][yMax][zMax] = 0;
        }

        // print trigrams
        for (int i=0; i<26*26; i++) {
            if (sortedTrigramsCount[i] <= 1) {
                break;
            }
            printf("%c%c%c %2d\n", sortedTrigrams[i][0], sortedTrigrams[i][1], sortedTrigrams[i][2], sortedTrigramsCount[i]);
        }

        // find distance between each of the most common trigrams
        int spots[4][7];
        int dists[4][6];
        for (int i=0; i<4; i++) {
            // I only care about the 4 most common trigrams
            //int spots[7] = {0,0,0,0,0,0,0};
            //int dists[6] = {0,0,0,0,0,0};
            int s = 0;
            // find trigram locations in string
            for (int j=2; j<strlen(argv[2])-1; j++) {
                if (argv[2][j-2] == sortedTrigrams[i][0]
                    && argv[2][j-1] == sortedTrigrams[i][1]
                    && argv[2][j] == sortedTrigrams[i][2]) {
                        spots[i][s] = j;
                        s++;
                    }
            }
            printf("%c%c%c ", sortedTrigrams[i][0], sortedTrigrams[i][1], sortedTrigrams[i][2]);
            for (int j=0; j<6; j++) {
                dists[i][j] = spots[i][j+1] - spots[i][j];
                printf("%4d,", dists[i][j]);
            }
            printf("\n");
        }

        // find common factors
        int factors[255];
        for (int i=0; i<254; i++) {
            factors[i] = 0;
        }
        // I know the smallest gap is 50 from prior code
        int isFactor = 1;
        int spot = 0;
        for (int k=1; k<50; k++) {
            for (int i=0; i<4; i++) {
                for (int j=0; j<6; j++) {
                    //printf("\n%d\n", dists[i][j]);
                    if (dists[i][j] % k != 0) {
                        //printf("\na\n");
                        isFactor = 0;
                        break;
                    }
                }
                if (!isFactor) {
                    break;
                }
            }
            if (isFactor) {
                factors[spot] = k;
                spot++;
            }
            isFactor = 1;
        }
        for (int i=0; i<255; i++) {
            if (factors[i] == 0) {
                break;
            }
            printf("%3d,", factors[i]);
        }
        printf("\n");


        //char* key = "SCKNEI_";
        char* key = "SCHNEIE";
        int whichChar = 0;
        int offset;
        char tmp;
        for (int i=0; i<26; i++) {
            printf("\n\n");
            printf("offset: %c\n\n", (char) (i+65));
            for (int j=0; j<strlen(argv[2])-1; j++) {
                offset = (int) key[whichChar];
                if (whichChar == 6) {
                    //offset = i + 65;
                }
                offset -= 65;
                tmp = argv[2][j];
                tmp -= 65;
                tmp = (tmp - offset) % 26;
                if (tmp < 0) {
                    tmp += 26;
                }
                tmp += 65;
                printf("%c", tmp);
                whichChar = (whichChar+1) % 7;
            }
            whichChar = 0;
        }
    } else {
        printf("valid decrypt options are:\n shift\n mono-sub\n vigen\n");
        return 1;
    }
    return -1;
}