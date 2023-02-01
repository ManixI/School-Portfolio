#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argv, char *argc[]) {
    int fp = open(argc[1], O_RDONLY);
    char *buffer = (char *) calloc(1, sizeof(char));
    int tmp;

    int end = lseek(fp, 0, SEEK_END);
    int position = end;

    for (int i = 0; i < end; i++) {
        tmp = read(fp, buffer, 1);
        write(STDOUT_FILENO, buffer, 1);
        position--;
        tmp = lseek(fp, position, SEEK_SET);
    }
    write(STDOUT_FILENO, "\n", 1);   
    close(fp);
    free(buffer);
    return 0;
}
