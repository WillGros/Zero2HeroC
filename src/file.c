#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"
#include "file.h"

int create_db_file(char *filename) {
  int fd = open(filename, O_RDWR, 0644);
  if(fd != -1){
    close(fd);
    printf("File: \"%s\" already exists.\n", filename);
    return STATUS_ERROR;
  }

  fd = open(filename, O_RDWR | O_CREAT, 0644);
  if(fd == -1){
    close(fd);
    perror("open");
    return STATUS_ERROR;
  }
  return 1;
}

int open_db_file(char *filename) {
  int fd = open(filename, O_RDWR, 0644);
  if(fd == -1){
    close(fd);
    perror("open");
    return STATUS_ERROR;
  }
  return 1;
}


