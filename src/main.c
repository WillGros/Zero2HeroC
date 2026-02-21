
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
}

int main(int argc, char *argv[]) {
  int c = 0; // default value for the switch case
  char *filepath = NULL; //default address for filepath ptr
  bool newfile = false; //default val for -n flag
  
  while ((c = getopt(argc, argv, "nf:") != -1)){
    switch(c){
      case 'n':
        newfile = true;
        break;
      case 'f':
        filepath = optarg;
        break;
      case '?':
        printf("Unknown option %c\n", c);
      default:
        return -1;
    }
  }

  if(filepath == NULL){
    printf("Filepath required.");
    print_usage(argv);
    
    return 0;
  }


  if(newfile){
    create_db_file(*filepath);

  }

  printf("New file: %b\n", newfile);
  printf("Filepath: %c\n", *filepath);

  return 0;
}
