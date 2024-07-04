#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

/**
 * EC code for creating files of specific sizes to test project 3
*/

#define MB 1000000

void
test_file(char* filename, int num_mb)
{
  char buf[1000];
  int fd, bytes;
  fd = open(filename, O_CREATE | O_WRONLY);
  if (fd < 0) {
    printf(2, "ec test: cannot open file for writing\n");
    exit();
  }
  bytes = 0;
  printf(2, "Writing MB x of %d: ", num_mb);
  while (bytes < (MB/1000) * num_mb) {
    *(int*) buf = bytes;
    int cc = write(fd, buf, sizeof(buf));
    if (bytes % (MB/1000) == 0) {
        printf(2, "%d ", bytes / (MB/1000));
        if ((bytes / (MB/1000)) % 10 == 0 && (bytes / (MB/1000)) % 10 > 0)
            printf(2, "\n");
    }
    if (cc <= 0)
        break;
    bytes++;
    
  }
  printf(2, "%d MB\n", bytes/1000);
  close(fd);

  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    printf(2, "ec test: cannot re-open file for reading\n");
    exit();
  }
  for (int i=0; i < (bytes/1000) * num_mb; i++) {
    int cc = read(fd, buf, sizeof(buf));
    if (cc<=0) {
      printf(2, "ec test: read error at read %d\n", i);
      exit();
    }
    if (*(int*)buf != i) {
      printf(2, "ec test: read wrong data (%d) for read %d\n", *(int*)buf, i);
      exit();
    }
  }
}

int
main()
{
  test_file("1mb.file", 1);
  test_file("4mb.file", 4);
  test_file("16mb.file", 16);
  test_file("64mb.file", 64);
  exit();
}
