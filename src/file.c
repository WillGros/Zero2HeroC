#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"
#include "file.h"

int fd = -1;

int create_db_file(char *filename) {
  fd = open(filename, O_RDONLY);
  if(fd != -1){
    printf("%c already exists.", *filename);
    return STATUS_ERROR;
  }

  fd = open(filename, O_RDWR | O_CREAT, 0664);
  if(fd == 1){
    perror("open");
    return STATUS_ERROR;
  }
  return 1;
}

int open_db_file(char *filename) {
  fd = open(filename, O_RDONLY);
  if(fd != -1){
    printf("%c already exists.", *filename);
    return STATUS_ERROR;
  }

  fd = open(filename, O_RDWR);
  if(fd == 1){
    perror("open");
    return STATUS_ERROR;
  }
  return 1;
}


