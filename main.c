#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#define LINE_SIZE 255

enum field_type_t {
  CSV_INT
  ,CSV_STRING
  ,CSV_EOF
};

struct field_t {
  void *data;
  size_t size;
  enum field_type_t type;
};

struct field_t *create_field(void *const bytes, size_t size, enum field_type_t type) {
  struct field_t *field=calloc(1, sizeof(struct field_t));
  if(type==CSV_EOF) {
    field->type=type;
    goto out;
  }
  field->type=type;
  field->data=calloc(1, size); 
  field->size=size;
  memcpy(field->data, bytes, size);

  out:
    return field;
}

void printf_field(struct field_t field) {
  printf("%.*s ||| ", field.size, (char*)(field.data));
}

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

void push_field_to_list(struct field_t *field, struct field_t ***fields, uint64_t size) {
  if(*fields==0) {
    *fields=calloc(1, sizeof(struct field_t *));
  }
  else {
    *fields=realloc(*fields, (size+1)*sizeof(struct field_t *));
  }
  *(*fields+size)=field;
}

struct field_t **lex_line(char *const line) {
  struct field_t **fields=0;
  const char *delims="\n,";
  char *start=line, *const end=line+strlen(line)+1;
  char str[LINE_SIZE];
  uint64_t size=0;
  while(start != end) {
    ssize_t match=strcspn(start, delims);
    memcpy(str, start, match);
    str[match]='\0';
    struct field_t *field=create_field(str, match, CSV_STRING);
    push_field_to_list(field, &fields, size++);
    start+=match+1;
  }
  push_field_to_list(create_field(0, 0, CSV_EOF), &fields, size++); 
  return fields;
}
// push_field_to_list(field, line_fields, size++);

void push_list_to_matrix(struct field_t **field_list, struct field_t ****matrix, uint64_t size) {
  if(*matrix==0) {
    *matrix=calloc(1, sizeof(struct field_t **));
  }
  else {
    *matrix=realloc(*matrix, (size+1)*sizeof(struct field_t **));
  }
  *(*matrix+size)=field_list;
}

struct field_t ***lex(const char *const file_name) {
  struct field_t ***result=0;
  char *content=0;
  ssize_t read_bytes=read_content(file_name, &content);
  const char *const delim=",", *const line_break="\n";
  char *start=content, *const end=content+read_bytes, *match=0;
  char line[LINE_SIZE];
  uint64_t size=0;
  while(start != end) {
    match=strstr(start, line_break);
    memcpy(line, start, match-start);
    line[match-start]='\0';
    struct field_t **line_fields=lex_line(line);
    push_list_to_matrix(line_fields, &result, size++);
    start=match+1;
  }
  free(content);
  push_list_to_matrix(0, &result, size++);
  return result;
}

int main(int argc, char **argv) {
  char *file_name="sample.csv";
  //struct field_t **matrix=lex(file_name);
  struct field_t ***lines=lex(file_name);
  struct field_t ***start=lines;
  printf("%p %p %p\n", start, *start, **start);
  while(*start != 0) {
    struct field_t **list=*start;
    while((*list)->type != CSV_EOF) {
      printf_field(**list);
      list++;
    }
    start++;
    printf("\n");
  }
  return EXIT_SUCCESS;
}
