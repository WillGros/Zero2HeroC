#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {

}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring) {

}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {

}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
  
  if(fd < 0){ // validate file descriptor of db file
    printf("Bad file descriptor.\n");
    return STATUS_ERROR;
  }

  dbhdr->magic = htonl(dbhdr->magic);
  dbhdr->filesize = htonl(dbhdr->filesize);
  dbhdr->count = htons(dbhdr->filesize);
  dbhdr->version = htons(dbhdr->version);

  lseek(fd, 0, SEEK_SET);

  write(fd, dbhdr, sizeof(struct dbheader_t));

  return STATUS_SUCCESS;
}	

int validate_db_header(int fd, struct dbheader_t **headerOut) {
  if(fd < 0){ // validate file descriptor of db file
    printf("Bad file descriptor.\n");
    return STATUS_ERROR;
  }

  struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t)); // create pointer to db header struct in heap
  if (header == NULL){ //validate heap allocation
    printf("malloc failed to create db header.\n");
    return STATUS_ERROR;
  }

  if(read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)){ //validate the dize of the db header read
    perror("read"); //return error if not;
    free(header); //free heap memory allocated by calloc
    return STATUS_ERROR;
  }

  header->version = ntohs(header->version); // unpack binary data from the header from BIG on disk to correct endianness of the system
  header->count = ntohs(header->count);
  header->magic = ntohl(header->magic);
  header->filesize = ntohl(header->filesize);

  if(header->version != 1){
    printf("Impropper header version.\n");
    free(header);
    return STATUS_ERROR;
  }
  if(header->magic != HEADER_MAGIC){
    printf("Impropper header magic.\n");
    free(header);
    return STATUS_ERROR;
  }
  struct stat dbstat = {0};
  fstat(fd, &dbstat);

  if(header->filesize != dbstat.st_size){
    printf("Database file is corrupted.\n");
    free(header);
    return STATUS_ERROR;
  }
  
  return STATUS_SUCCESS;
}

int create_db_header(int fd, struct dbheader_t **headerOut) {
  struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t)); // create pointer db header struct in heap
  if(header == NULL){ //check heap allocation
    printf("malloc failed to create db header\n"); //return error if malloc fails
    return STATUS_ERROR;
  }
  header->version = 1;  //initialize header values
  header->count = 0;
  header->magic = HEADER_MAGIC;
  header->filesize = sizeof(struct dbheader_t);

  *headerOut = header; //write ptr header out to dptr headerOut

  return STATUS_SUCCESS;
}


