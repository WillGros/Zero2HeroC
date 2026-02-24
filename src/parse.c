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
  if(dbhdr == NULL || employees == NULL){
    printf("List employees recieved a NULL pointer.\n");
    return;
  }
  int i = 0;
  for(; i < dbhdr->count; i++){
    printf("(%d) %s\n  - Address: %s\n  - Hours: %d\n", (i+1), employees[i].name, employees[i].address, employees[i].hours);
  }
}


int add_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char *addstring) {
  
  if(dbhdr == NULL) return STATUS_ERROR;
  if(employees == NULL) return STATUS_ERROR;
  if(*employees == NULL) return STATUS_ERROR;
  if(addstring == NULL) return STATUS_ERROR;
  

  char *name = strtok(addstring, ",");
  char *address = strtok(NULL, ",");
  char *hours = strtok(NULL, ",");

  if(name == NULL) return STATUS_ERROR;
  if(address == NULL) return STATUS_ERROR;
  if(hours == NULL) return STATUS_ERROR;
  
  struct employee_t *empTemp = *employees;
  empTemp = realloc(empTemp, sizeof(struct employee_t)*(dbhdr->count+1));
  if(empTemp == NULL){
    perror("malloc");
    return STATUS_ERROR;
  }

  dbhdr->count++;

  strncpy(empTemp[dbhdr->count-1].name, name, sizeof(empTemp[dbhdr->count-1].name)-1);

  strncpy(empTemp[dbhdr->count-1].address, address, sizeof(empTemp[dbhdr->count-1].address)-1);

  empTemp[dbhdr->count-1].hours = atoi(hours);

  *employees = empTemp;

  return STATUS_SUCCESS;
}


int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {
  if(fd < 0){
    printf("Bad file descriptor.\n");
    return STATUS_ERROR;
  }

  int eCount = dbhdr->count;
  struct employee_t *employees = calloc(eCount, sizeof(struct employee_t));
  if(employees == -1){
    printf("Malloc failed.\n");
    return STATUS_ERROR;
  }

  read(fd, employees, eCount*sizeof(struct employee_t));

  int i = 0;
  for(; i < eCount; i++){
    employees[i].hours = ntohl(employees[i].hours);
  }

  *employeesOut = employees;
  return STATUS_SUCCESS;
}


int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
  if(fd < 0){ // validate file descriptor of db file
    printf("Bad file descriptor.\n");
    return STATUS_ERROR;
  }

  int eCount = dbhdr->count; // preserve real count value before packing

  dbhdr->magic = htonl(dbhdr->magic);
  dbhdr->filesize = htonl(sizeof(struct dbheader_t)+(sizeof(struct employee_t)*eCount));
  dbhdr->count = htons(dbhdr->count);
  dbhdr->version = htons(dbhdr->version);

  lseek(fd, 0, SEEK_SET);

  write(fd, dbhdr, sizeof(struct dbheader_t));

  int i = 0;
  for(; i < eCount; i++){
    employees[i].hours = htonl(employees[i].hours);
    write(fd, &employees[i], sizeof(struct employee_t));
  }
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

  if(header->version != HEADER_VERSION){
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
  
  *headerOut = header;
  return STATUS_SUCCESS;
}


int create_db_header(struct dbheader_t **headerOut) {
  struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t)); // create pointer db header struct in heap
  if(header == -1){ //check heap allocation
    printf("malloc failed to create db header\n"); //return error if malloc fails
    return STATUS_ERROR;
  }

  header->version = HEADER_VERSION;  //initialize header values
  header->count = 0;
  header->magic = HEADER_MAGIC;
  header->filesize = sizeof(struct dbheader_t);

  *headerOut = header; //write ptr header out to dptr headerOut

  return STATUS_SUCCESS;
}


