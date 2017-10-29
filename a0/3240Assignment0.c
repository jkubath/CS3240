/*
	Jonah Kubath
	CS3240
	9/5/2017
	a0
*/

/*
  SUMMARY:
    3240Assignment0 reads through the SongCSV.csv
    It then saves artist, nameOfSong, albumName, duration, releaseYear,
    and hotttnesss to a Struct
    It sorts the data by nameOfSong
    The user is then allowed to search by Song Name with ZZZ terminating the program
*/
#define _BSD_SOURCE
#include "apue.h"

//Struct for each Song
typedef struct {
  char *artist;
  char *nameOfSong;
  char *albumName;
  float duration;
  int releaseYear;
  double hot;

} Song;

int mySort(int count);
void swap(int a);
void search(int length);
int binarySearch(char *searchTitle, int begin, int middle, int end);
//Holds all the songs read from the File
Song **songs;

int main(void){
  int arrayLength = 1000;
  int arrayLengthExtension = 100;
  int arrayLengthExtensionCount = 1;
  int tempArrayLength = 5000;
  int numberOfFields = 18;
  char *tok;
  int tokenCount = 0;
  //Open file
  char fileName[] = "SongCSV.csv";
  FILE *file = fopen(fileName, "r");
  songs = malloc(sizeof(Song*) * arrayLength);
  if(songs == NULL){
    err_sys("Memory Allocation Failed");
  }

  //Count where to add the next struct
  int count = 0;
  //File does not exist
  if(file == NULL){
    err_sys("Bad FileName");
  }
  else{
    if(printf("Reading / Sorting Song File\n") == 0){
      err_sys("Printing Failed");
    }
    //Consume the first line of the file
    char **tempSong = malloc(sizeof(char*) * numberOfFields);
    if(tempSong == NULL){
      err_sys("Memory Allocation Failed");
    }
    char *tempLine = malloc(sizeof(char) * tempArrayLength);
    if(tempLine == NULL){
      err_sys("Memory Allocation Failed");
    }
    if(fgets(tempLine, tempArrayLength, file) == NULL){
      err_sys("Failed to get a line");
    };

    free(tempLine);
    free(tempSong);
    //Read through the file
    while(feof(file) == 0){
      //Realloc memory if more songs are read than the array has space for
      if(arrayLength == count){
        arrayLength++;
        songs = realloc(songs, sizeof(Song*) * (arrayLength + (arrayLengthExtension * arrayLengthExtensionCount)));
        if(songs == NULL){
          err_sys("Memory Re Allocation Failed");
        }
        arrayLengthExtensionCount++;
      }
      //Temporary song
      char **tempSong = malloc(sizeof(char*) * numberOfFields);
      if(tempSong == NULL){
        err_sys("Memory Allocation Failed");
      }
      //Temporary line
      char *tempLine = malloc(sizeof(char) * tempArrayLength);
      if(tempLine == NULL){
        err_sys("Memory Allocation Failed");
      }
      //Read the line
      fgets(tempLine, tempArrayLength, file)
      // Keep track of which position the program is at
      // For the temporarySong
      if(strlen(tempLine) == 0){
        //Do nothing if a null string is read
      }
      else{
        //Reset tokenCount to beginning of tempSong array
        tokenCount = 0;
        while((tok = strsep(&tempLine, ",")) != NULL){
          //Save the broken off string to tempSong
          tempSong[tokenCount] = malloc((strlen(tok) * 2) + 1);
          if(tempSong[tokenCount] == NULL){
            err_sys("Memory Allocation Failed");
          }
          strcpy(tempSong[tokenCount], tok);
          tokenCount++;
        }

      //Add to array of songs
      //Give space for the struct
      songs[count] =(Song*) malloc(sizeof(Song));
      if(songs[count] == NULL){
        err_sys("Memory Allocation Failed");
      }
      //Give space for the Song name
      songs[count] -> nameOfSong = malloc((strlen(tempSong[17]) * 2) + 1);
      if(songs[count] -> nameOfSong == NULL){
        err_sys("Memory Allocation Failed");
      }
      strncpy(songs[count] -> nameOfSong, tempSong[17], (strlen(tempSong[17]) + 1));
      if(songs[count] -> nameOfSong == NULL){
        err_sys("Failed to Copy - Name Of Song");
      }
      //Give space for the artist name
      songs[count] -> artist = malloc((strlen(tempSong[8]) * 2) + 1);
      if(songs[count] -> artist == NULL){
        err_sys("Memory Allocation Failed");
      }
      strncpy(songs[count] -> artist, tempSong[8], (strlen(tempSong[8]) + 1));
      if(songs[count] -> artist == NULL){
        err_sys("Failed to Copy - Artist Name");
      }
      //Give space for the album name
      songs[count] -> albumName = malloc((strlen(tempSong[3]) * 2) + 1);
      if(songs[count] -> albumName == NULL){
        err_sys("Memory Allocation Failed");
      }
      strncpy(songs[count] -> albumName, tempSong[3], (strlen(tempSong[3]) + 1));
      if(songs[count] -> albumName == NULL){
        err_sys("Failed to Copy - Album Name");
      }
      //Give space for the duration
      songs[count] -> duration = atof(tempSong[10]);
      //Give space for the release year
      songs[count] -> releaseYear = atoi(tempSong[18]);
      //Give space for the hotttnesss
      if(strcmp(tempSong[14], "nan") == 0){
        songs[count] -> hot = 0.0;
      }
      else
        songs[count] -> hot = strtod(tempSong[14], NULL);
      //Sort the struct by Song Name
      if(count > 0){
        mySort(count);
      }
      //Increment the spot to input the next struct
      count++;
      //Release the tempLine
      free(tempLine);
      free(tempSong);
      }
    }//End of file
  }//End of else statement

  //Allow the user to search songs
  search(count - 1);

  fclose(file);
  return 0;

}//End of Main

//Used to sort the Songs Structs as the data is read
int mySort(int count){
    //Holds the bounds of the array
    int outOfBounds = count;
    int result = strcasecmp(songs[outOfBounds] -> nameOfSong, songs[outOfBounds - 1] -> nameOfSong);
    while(result < 0 && outOfBounds > 0){
      swap(outOfBounds);
      //Decrement the position to swap
      outOfBounds = outOfBounds - 1;
      if(outOfBounds > 0){
        result = strcasecmp(songs[outOfBounds] -> nameOfSong, songs[outOfBounds - 1] -> nameOfSong);
      }
    }

    return 0;
  }

//Used to Bubble Sort as Structs are created
  void swap(int a){
    int b = a - 1;
    //Temp pointer to a Song struct
    Song *tempStruct = malloc(sizeof(*songs[a]));
    if(tempStruct == NULL){
      err_sys("Memory Allocation Failed");
    }
    //Copy the struct from A
    *tempStruct = *songs[a];
    //Copy the struct from B to A
    //Re allocate size
    songs[a] = realloc(songs[a], sizeof(*songs[b]));
    if(songs[a] == NULL){
      err_sys("Memory Allocation Failed");
    }
    *songs[a] = *songs[b];
    //Re apply the struct from tempStruct
    songs[a-1] = realloc(songs[a-1], sizeof(*tempStruct));
    if(songs[a-1] == NULL){
      err_sys("Memory Allocation Failed");
    }
    *songs[a-1] = *tempStruct;
    //Free the memory
    free(tempStruct);
  }

//User Search
void search(int length){
  char *cmd_line = malloc(sizeof(char) * 100);
  if(cmd_line == NULL){
    err_sys("Memory Allocation Failed");
  }
  char *exitString = "\"ZZZ\"";
  int compare = -1;
  printf("Enter a Song Title (ZZZ to stop)\n");
  while(compare != 0){
    fgets(cmd_line ,100, stdin);
    char *searchWord = malloc(strlen(cmd_line) + 1);
    if(searchWord == NULL){
      err_sys("Memory Allocation Failed");
    }
    searchWord[0] = '\"';
    int size = strlen(cmd_line);
    //Null terminate
    cmd_line[size - 1] = '\0';
    strcat(searchWord, cmd_line);
    searchWord[strlen(searchWord)] = '\"';
    //Flush the buffer
    fflush(stdin);
    compare = strcmp(searchWord, exitString);
    int found = -1;
    if(compare != 0){
      found = binarySearch(searchWord, 0, length / 2, length);
    if(found > 0){
      printf("Song         - %s\n", songs[found] -> nameOfSong);
      printf("Album        - %s\n", songs[found] -> albumName);
      printf("Artist       - %s\n", songs[found] -> artist);
      printf("Duration     - %f\n", songs[found] -> duration);
      printf("Release Year - %d\n", songs[found] -> releaseYear);
      printf("Hotttnesss   - %f\n\n", songs[found] -> hot);
    }
    else
      printf("\nSong not found.\n\n");
    }
  }
}

int binarySearch(char *searchTitle, int begin, int middle, int end){
  int found = -1;
  int compare = strcasecmp(searchTitle, songs[middle] -> nameOfSong);
  if(compare == 0){
    return middle;
  }
  //Search Title is between the middle and end
  else if(compare > 0 && begin != middle){
    found = binarySearch(searchTitle, middle, (end + middle) / 2, end);
  }
  //Search Title is between the beginning and middle
  else if(compare < 0 && begin != middle){
    found = binarySearch(searchTitle, begin, (begin + middle) / 2, middle);
  }
  //Not Found
  else
    return -1;

  return found;
}
