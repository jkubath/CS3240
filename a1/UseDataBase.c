/*
	Jonah Kubath
	CS3240
	9/19/2017
	a1
*/

/*
  SUMMARY:

*/

#define _BSD_SOURCE
#include "apue.h"
#include <fcntl.h>

void search(char * songDatabase, int length);
int binarySearch(char *searchTitle, int begin, int end);
void getStruct(int file, long offset);

//Struct for each Song
typedef struct {
  char *nameOfSong;
  long offSet;

} Song;
//Holds the song structs
Song **songArray;


int main(void){
  int extendArray = 0;
  int arrayLength = 100;
  songArray = calloc(arrayLength, sizeof(Song *));
  char *songDirectory = "songDirectory.bin";
  char *songDatabase = "songDatabase.bin";
  //char *bufferArray = malloc(1000);
  int arrayIndex = 0;

  //Open the file in read only
  int file = open(songDirectory, O_RDONLY);
  if(file == -1){
    err_sys("Song Directory does not exist.\n");
  }

  //Holds the size of the next string
  int size = 1;
  //Holds the string read
  char *string;
  //Holds the offSetSize
  long offSetSize = 0;

  //Read the file
  while(size > 0){
    //Stop loop
    read(file, &size, sizeof(int));
    if(size > 0){
      if(arrayIndex >= arrayLength){
        extendArray++;
        songArray = realloc(songArray, (arrayLength + (extendArray * arrayLength)) * sizeof(Song*));
      }
      songArray[arrayIndex] = calloc(1, sizeof(Song));
    //Read the size of the string

    //Give space for string
    string = calloc(1, size + 1);
    if(string != NULL){
      read(file, string, size);
      read(file, &offSetSize, sizeof(long));
      songArray[arrayIndex] -> nameOfSong = calloc(1, size + 1);
      strcpy(songArray[arrayIndex] -> nameOfSong, string);
      //printf("%ld - %s\n", offSetSize, string);
      songArray[arrayIndex] -> offSet = offSetSize;
      arrayIndex++;
    }
    else
    {
      free(songArray[arrayIndex]);
    }
    free(string);
  }//End of if statement
}//End of while loop

  // for(int i = 0; i < arrayIndex; i++){
  //   printf("%ld - %s\n", songArray[i] -> offSet, songArray[i] -> nameOfSong);
  // }

  search(songDatabase, arrayIndex);

  //Free Array
  for(int i = 0; i < arrayIndex; i++){
    free(songArray[i] -> nameOfSong);
    free(songArray[i]);
  }
  free(songArray);

  printf("%s\n", "Done Searching");

  close(file);
}//End of main

//User Search
void search(char * songDatabase, int length){
  // //Open the file for structs in read only
  int database = open(songDatabase, O_RDONLY);
  if(database == -1){
    err_sys("Song Database does not exist.\n");
  }


  char *exitString = "\"ZZZ\"";
  int compare = -1;

  while(compare != 0){
    char *cmd_line = calloc(1, 100);
    if(cmd_line == NULL){
      err_sys("Memory Allocation Failed - CMD Line");
    }
    printf("Enter a Song Title (ZZZ to stop)\n");
    //read(STDIN_FILENO, cmd_line, sizeof(cmd_line));
    gets(cmd_line);
    char *searchWord = calloc(1, strlen(cmd_line) + 1);
    if(searchWord == NULL){
      err_sys("Memory Allocation Failed");
    }
    searchWord[0] = '\"';
    //int size = strlen(cmd_line);

    strcat(searchWord, cmd_line);
    searchWord[strlen(searchWord)] = '\"';
    printf("You entered: %s\n", searchWord);
    //Flush the buffer
    fflush(stdin);
    compare = strcmp(searchWord, exitString);
    int found = -1;
    if(compare != 0){
      found = binarySearch(searchWord, 0, length);
    if(found > 0){
      getStruct(database, (long) songArray[found] -> offSet);
    }
    else
      printf("\nSong not found.\n\n");
    }
    //Free allocated space
    free(searchWord);
    free(cmd_line);
  }
  close(database);
}

int binarySearch(char *searchTitle, int begin, int end){
  int found = -1;
  int middle = (begin + end) / 2;
  //printf("Search - %s\n", songArray[middle] -> nameOfSong);
  int compare = strcasecmp(searchTitle, songArray[middle] -> nameOfSong);
  if(compare == 0){
    return middle;
  }
  else if(begin > end){
    return -1;
  }
  //Search Title is between the middle and end
  else if(compare > 0){
    found = binarySearch(searchTitle, middle + 1,  end);
  }
  //Search Title is between the beginning and middle
  else if(compare < 0){
    found = binarySearch(searchTitle, begin, middle - 1);
  }
  //Not Found
  else
    return -1;

  return found;
}

void getStruct(int file,long offset){

  char *string;
  float floatValue;
  double doubleValue;
  int size = 0;
  lseek(file,(long) 0, 0);
  if(lseek(file, (long) offset, 0) < 0) {
    err_sys("Error in lseek");
  }
  //Read the integer
  read(file, &size, sizeof(int));
  string = calloc(1, size + 1);
  read(file, string, size);
  printf("Name         - %s\n", string);
  free(string);
  //Read the integer
  read(file, &size, sizeof(int));
  string = calloc(1, size + 1);
  read(file, string, size);
  printf("Artist       - %s\n", string);
  free(string);
  //Read the integer
  read(file, &size, sizeof(int));
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
