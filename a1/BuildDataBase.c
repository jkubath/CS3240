/*
	Jonah Kubath
	CS3240
	9/5/2017
	a1
*/

/*
  SUMMARY:
    Read SongCSV.csv and make a database of useful information along with a
    directory that will hold the song name and the offset in the database.


*/
#define _BSD_SOURCE
#include "apue.h"
#include <fcntl.h>

//Struct for each Song
typedef struct {
  char *artist;
  char *nameOfSong;
  char *albumName;
  int releaseYear;
  float duration;
  double hot;

} Song;
//Holds the song structs
Song **songArray;

int mySort(int n);
void writeStringToFile(int, char*);
void writeIntToFile(int, int);
void writeLongToFile(int writeFile, long number);
void writeNumbersToFile(int writeFile, int integer, float floatValue, double doubleValue);

int main(void){
  int arrayLength = 500;
  int expandArrayLength = 100;
  int expandArrayCount = 0;
  int BUFFER_SIZE = 4096;
  //int tempArrayLength = 500;
  char *bufferArray = calloc(1, BUFFER_SIZE + 1);
  char *tempLine;
  char *tempSong;
  //char *tempString;
  //char *transferString;
  char *songData;
  int bufferIndex = 0;
  int breakWhile = 0;
  int structCount = 0;
  int structIndex = 0;
  char *songDatabase = "songDatabase.bin";
  char *songDirectory = "songDirectory.bin";
  int directory;
  int database;

  //Open the file to read from
  int file = open("SongCSV.csv", O_RDONLY);
  if(file == -1){
    err_sys("File did not open\n");
  }

  //Open the file to write to
  if( access( songDatabase, F_OK ) != -1 ) {
    // file exists
    database = open(songDatabase, O_RDWR);
  } else {
    // file doesn't exist
    database = open(songDatabase, O_CREAT | O_RDWR);
  }
  //Open the file to write to
  if(database == -1){
    err_sys("Creating songDatabase failed\n");
  }

  //Open the file to write to
  if( access(songDirectory, F_OK ) != -1 ) {
    // file exists
    directory = open(songDirectory, O_RDWR);
  } else {
    // file doesn't exist
    directory = open(songDirectory, O_CREAT | O_RDWR);
  }
  //Open the file to write to
  if(directory == -1){
    err_sys("Creating songDirectory failed\n");
  }

  //Create my array to holds the songs
  songArray = calloc(arrayLength, sizeof(Song*));
  if(songArray == NULL){
    err_sys("Memory Allocation Failed");
  }

  printf("Processing Data\n");
  //Read a buffer from the file
  char *tempBuffer;
  while(read(file, bufferArray + bufferIndex, BUFFER_SIZE - bufferIndex) != 0 ) {
    //Save the broken off string to tempBuffer
    tempBuffer = bufferArray;
    //Seperate \n
    while(breakWhile == 0 && (tempLine = strsep(&tempBuffer, "\n")) != NULL){
      if(tempBuffer == NULL && tempLine != NULL){
        //Copy extra code to buffer
        strcpy(bufferArray, tempLine);
        bufferIndex = strlen(tempLine);
        breakWhile = 1;
      }
      else {
        bufferIndex = 0;
      }
      //Save the broken off string to tempSong
      //Break by comma
      //Reset counter
      structCount = 0;
      //Expand array if necessary
      if(structIndex >= arrayLength){
        expandArrayCount++;
        songArray = realloc(songArray,
          (arrayLength + expandArrayCount * expandArrayLength) * sizeof(Song));
      }
      //Allocate for struct
      songArray[structIndex] =(Song *) calloc(1, sizeof(Song));
      if(songArray[structIndex] == NULL){
        err_sys("Memory Allocation Failed for new Struct");
      }
      tempSong = tempLine;
      while(breakWhile == 0 && (songData = strsep(&tempSong, ",")) != NULL){
        //Holds each set of data - artist name, song name, duration
          switch(structCount){
            case 3:
              //Give space for the album name
              songArray[structIndex] -> albumName = calloc(1, (strlen(songData) + 1));
              if(songArray[structIndex] -> albumName == NULL){
                err_sys("Memory Allocation Failed - Album Name");
              }
              strncpy(songArray[structIndex] -> albumName, songData, strlen(songData));
              if(songArray[structIndex] -> albumName == NULL){
                err_sys("Failed to Copy - Album Name");
              }
              break;
            case 8:
              //Give space for the artist name
              songArray[structIndex] -> artist = calloc(1, (strlen(songData) + 1));
              if(songArray[structIndex] -> artist == NULL){
                err_sys("Memory Allocation Failed - Artist Name");
              }
              strncpy(songArray[structIndex] -> artist, songData, (strlen(songData)));
              if(songArray[structIndex] -> artist == NULL){
                err_sys("Failed to Copy - Artist Name");
              }
              break;
            case 10:
              //Copy the duration
              songArray[structIndex] -> duration = atof(songData);
              break;
            case 14:
              //Copy the hotttnesss
              if(strcmp(songData, "nan") == 0){
                songArray[structIndex] -> hot = 0.0;
              }
              else
                songArray[structIndex] -> hot = strtod(songData, NULL);
              break;
            case 17:
              //Give space for the Song name
              songArray[structIndex] -> nameOfSong = calloc(1, (strlen(songData) + 1));
              if(songArray[structIndex] -> nameOfSong == NULL){
                err_sys("Memory Allocation Failed");
              }
              strncpy(songArray[structIndex] -> nameOfSong, songData, (strlen(songData)));
              if(songArray[structIndex] -> nameOfSong == NULL){
                err_sys("Failed to Copy - Name Of Song");
              }
              break;
            case 18:
              //Give space for the release year
              songArray[structIndex] -> releaseYear = atoi(songData);
              break;
            default:
              break;
          }
          structCount++;

      }//End of individual data

      //structIndex++;
      //If data was null - free space allocated to it
      if(songArray[structIndex] -> nameOfSong == NULL){
        free(songArray[structIndex] -> nameOfSong);
        free(songArray[structIndex] -> artist);
        free(songArray[structIndex] -> albumName);
        free(songArray[structIndex]);
      }
      else {
        structIndex++;
      }
      //Reset - broken buffer
      breakWhile = 0;

    }//End of break new line
  }//End of reading file
  close(file);
  //Sort array of Songs
  mySort(structIndex);

  //Free the header of the file
  structIndex--;
  free(songArray[structIndex] -> nameOfSong);
  free(songArray[structIndex] -> artist);
  free(songArray[structIndex] -> albumName);
  free(songArray[structIndex]);

  long offSetSize = 0;
  for(int i = 0; i < structIndex; i++){
    //Write data to the database
    writeStringToFile(database, songArray[i] -> nameOfSong);
    writeStringToFile(database, songArray[i] -> artist);
    writeStringToFile(database, songArray[i] -> albumName);
    writeNumbersToFile(database, songArray[i] -> releaseYear, songArray[i] -> duration, songArray[i] -> hot);
    //Size of the name -> the song name -> offset of database file
    writeStringToFile(directory, songArray[i] -> nameOfSong);
    writeLongToFile(directory,(long) offSetSize);
    //printf("%ld - %s\n", offSetSize, songArray[i] -> nameOfSong);
    //Write data to the directory
    offSetSize += strlen(songArray[i] -> nameOfSong) + strlen(songArray[i] -> artist)
     + strlen(songArray[i] -> albumName) + (4 * sizeof(int)) + sizeof(float) +
     sizeof(double);

  }

  writeIntToFile(directory, -1);
  //Close the read in file
  close(database);
  close(directory);

  //Print data and free space
  for(int i = 0; i < structIndex; i++){
    free(songArray[i] -> nameOfSong);
    free(songArray[i] -> artist);
    free(songArray[i] -> albumName);
    free(songArray[i]);

  }

  free(songArray);
  free(bufferArray);

  printf("Database Built\n");
  return 0;
}//End of Main

int mySort(int n){
  int i,j;
  Song *tempStruct;
  for (i = 0; i < n - 1 ; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            if (strcmp(songArray[i] -> nameOfSong, songArray[j] ->nameOfSong) > 0)
            {
                tempStruct = songArray[i];
                songArray[i] = songArray[j];
                songArray[j] = tempStruct;

            }
        }
    }
    return 0;
}


  //Write a string to file
  void writeStringToFile(int writeFile, char* string){
    //Write string length to file
    int size = strlen(string);
    writeIntToFile(writeFile, size);
    if(write(writeFile, string, size) != strlen(string)) {
      err_sys("Failed writing a string to songDatabase");
    }
  }

  //Write an integer to file
  void writeIntToFile(int writeFile, int number){
    if(write(writeFile, &number, sizeof(int)) != sizeof(int)) {
      err_sys("Failed writing an integer to songDatabase");
    }
  }

  //Write an integer to file
  void writeLongToFile(int writeFile, long number){
    if(write(writeFile, &number, sizeof(long)) != sizeof(long)) {
      err_sys("Failed writing a long to songDatabase");
    }
  }

  void writeNumbersToFile(int writeFile, int integer, float floatValue, double doubleValue){
    if(write(writeFile, &integer, sizeof(int)) != sizeof(int)) {
      err_sys("Failed writing an integer to songDatabase");
    }
    if(write(writeFile, &floatValue, sizeof(float)) != sizeof(float)) {
      err_sys("Failed writing a float to database");
    }
    if(write(writeFile, &doubleValue, sizeof(double)) != sizeof(double)) {
      err_sys("Failed writing an double to database");
    }
  }
