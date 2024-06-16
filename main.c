#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void lex_line(const char *const line) {
  printf("%s\n", line);
}

void lex(const char *const file_name) {
  char *content=0;
  ssize_t read_bytes=read_content(file_name, &content);
  const char *const delim=",", *const line_break="\n";
  char *start=content, *const end=content+read_bytes;
  char *match=0;
  char line[1000]={0};
  while(start != end) {
    match=strstr(start, line_break);
    memcpy(line, start, match-start);
    line[match-start]='\0';
    lex_line(line);
    start=match+1;
  }
  free(content);
}

int main(int argc, char **argv) {
  char *file_name="sample.csv";
  lex(file_name);
  return EXIT_SUCCESS;
}
