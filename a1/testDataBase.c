

#define _BSD_SOURCE
#include "apue.h"
#include <fcntl.h>
#include<stdio.h>

int main(){
  char *songDatabase = "songDatabase.bin";
//Open the file for structs in read only
int file = open(songDatabase, O_RDONLY);
if(file == -1){
  err_sys("Song Database does not exist.\n");
}
  lseek(file,(long) 846740, 0);

for(int i = 0; i < 1; i++){

  char *string;
  float floatValue;
  double doubleValue;
  int size = 0;
  //Read the integer
  read(file, &size, sizeof(int));
  //size = 13;
  printf("Int - %d\n", size);
  string = calloc(1, size + 1);
  read(file, string, size);
  printf("Name         - %s\n", string);
  free(string);
  //Read the integer
  read(file, &size, sizeof(int));
  printf("Int - %d\n", size);
  string = calloc(1, size + 1);
  read(file, string, size);
  printf("Artist       - %s\n", string);
  free(string);
  //Read the integer
  read(file, &size, sizeof(int));
  printf("Int - %d\n", size);
  string = calloc(1, size + 1);
  read(file, string, size);
  printf("Album Name   - %s\n", string);
  free(string);
  //Read the integer
  read(file, &size, sizeof(int));
  printf("Release Year - %d\n", size);
  read(file, &floatValue, sizeof(float));
  printf("Duration     - %f\n", floatValue);
  read(file, &doubleValue, sizeof(double));
  printf("Hotttnesss   - %f\n\n", doubleValue);
}
  return 0;
}//end of main
