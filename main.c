#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>

ssize_t read_content(const char *const file_name, char **const buff) {
  int fd=open(file_name, O_RDONLY);
  uint64_t size=lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  *buff=calloc(1, size+1);
  ssize_t read_bytes=read(fd, *buff, size);
  (*buff)[size]='\0';
  close(fd);
  return size;
}

int main(int argc, char **argv) {
  char *file_name="sample.csv";
  char *content=0;
  ssize_t read_bytes=read_content(file_name, &content);
  printf("%s\n", content);
  free(content);
  return EXIT_SUCCESS;
}
