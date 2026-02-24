
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
  printf("Usage: %s -n -f <db file>\n", argv[0]);
  printf("\t -f (required) /path/to/file\n");
  printf("\t -n create new db file\n");
}

int main(int argc, char *argv[]) {
  int c = 0; // default value for the switch case
  char *filepath = NULL; //default address for filepath ptr
  bool newfile = false; //default val for -n flag
  
  int dbfd = -1; // default db fd value
  struct dbheader_t *header = NULL; //create dbheader struct as pointer and null it
  struct employee_t *employees = NULL;

  while ((c = getopt(argc, argv, "nf:")) != -1){
    switch(c){
      case 'n':
        newfile = true;
        break;
      case 'f':
        filepath = optarg;
        break;
      case '?':
        printf("Unknown option %c\n", c);
        break;
      default:
        return -1;
    }
  }

  if(filepath == NULL){
    printf("Filepath required.\n");
    print_usage(argv);
    
    return 0;
  }

  if(newfile){
    printf("New file: %b\n", newfile);
  }
  printf("Filepath: %s\n", filepath);
  
  if(newfile){
    dbfd = create_db_file(filepath);
    if(dbfd == STATUS_ERROR){
      printf("Unable to create new db file.\n");
      return -1;
    }
    if(create_db_header(&header) == STATUS_ERROR){
      printf("Failed to create DB header.\n");
      return -1;
    }else{
      output_file(dbfd, header, employees);
    }
  }else{
    dbfd = open_db_file(filepath);
    if(dbfd == STATUS_ERROR){
      printf("Unable to open db file.\n");
      return -1;
    }
    if(validate_db_header(dbfd, &header) == STATUS_ERROR){
      printf("Failed to valdate \"%s\" header.\n", filepath);
      return -1;
    }else{
      printf("Header successfully validated.\n");
    } 
  }

  if(read_employees(dbfd, header, &employees) == STATUS_ERROR){
    printf("Unable to read employees.\n");
    return -1;
  }

  output_file(dbfd, header, employees);
  
  return 0;
}
