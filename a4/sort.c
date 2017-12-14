/*
	Jonah Kubath
	CS3240
	11/18/2017
	a4

  SUMMARY:
    Our data base was destroyed and the data was spread out between multiple
    files.  This application takes in the directory with the data.
    Threads are then created for each file.  The files are read into structs
    and sorted.  The sorted arrays are then merged back into one file.
    sorted.yay
*/

#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include "apue.h"

//Nodes for the linked list of directories
typedef struct record_t record_t;

void printALine(FILE *file);
void printStruct(record_t record);
void *eachThread(void *fileName);
int compareRecords(const void *left, const void *right);

struct record_t {
  char *username;
  char *password;
  char *blood;
  char *domain;
  int index;
};

//Directory to read the files from
DIR *dp;
int defaultRecords = 500;
int defaultLineLength = 1000;
int total = 0;
int currentRecord = 0;
record_t **sortedFiles;
int *recordLength;
int totalRecords = 0;
int main(int argc, char** argv){
  char *outputFile = "sorted.yay";
  int err;
  int num_files = 0;
  char **file;

  if(argc < 2){
    printf("Invalid arguments.\n");
    printf("Invalid arguments.\t./3240Assignment4 <directory>\n");
  }
  //Valid arguments
  else {
    //Open a new directory
    struct stat statBuff;
    //Information from a directory
    struct dirent * dirp;
    //print directory name
    printf("Data Folder: %s\n", argv[1]);
    //Change directory to the user input
    if(chdir(argv[1]) == -1){
      printf("Error changing to %s\n", argv[1]);
    };

    //Open the directory from arguments
    dp = opendir(".");
    if(dp == NULL){
      err_sys("%s%s\n", "No read rights - ", argv[1]);
    }

    //Read the files inside the directory
    while((dirp = readdir(dp)) != NULL){
      //Gets the information from the directory and saves it to statBuff
      if(lstat(dirp -> d_name, &statBuff) == -1){
        printf("Error retrieving %s information\n", dirp -> d_name);
      };
      //Is it a regular file
      if (S_ISREG(statBuff.st_mode)){
        if(strcmp(dirp -> d_name, ".DS_Store") == 0 || strcmp(dirp -> d_name, "sorted.yay") == 0){
          //Do nothing
        }
        else {
          //Count the files
          num_files++;
        }
      }//End of a regular file
    }//End of while for reading current directory

    err = closedir(dp);
    if(err == -1){
      err_sys("Error closing the directory");
    }
    //Set pointer to beginning of the directory
    dp = opendir(".");
    if(dp == NULL){
      err_sys("Error opening the directory");
    }

    //Create an array of file names
    file = calloc(num_files, sizeof(char *));
    if(file == NULL){
      err_sys("Error allocating memory for the file array");
    }

    int i = 0;
    int recordCounter[num_files];
    while(i < num_files){
      recordCounter[i] = 0;
      //Allocate memory for each file pointer
      if((dirp = readdir(dp)) == NULL) {
        err_sys("Error retrieving file information");
      }
      //Gets the information from the directory and saves it to statBuff
      if(lstat(dirp -> d_name, &statBuff) == -1){
        printf("Error retrieving %s information\n", dirp -> d_name);
      };

      //Only read regular files
      //Otherwise skip
      if(strcmp(dirp -> d_name, "sorted.yay") != 0 && strcmp(dirp -> d_name, ".DS_Store")){
      if (S_ISREG(statBuff.st_mode)){
        //Save the file name
        file[i] = calloc(1, sizeof(strlen(dirp-> d_name)));
        //Copy over the file name
        strcpy(file[i], dirp -> d_name);
        i++;
      }
    }
  }

    //make the p threads in a for loop and join them
    pthread_t threads[num_files];

    //Array to hold the sorted file arrays
    sortedFiles = calloc(num_files, sizeof(record_t *));
    //The number of records read from the files
    recordLength = calloc(num_files, sizeof(int));
    currentRecord = 0;

    for(int i = 0; i < num_files; i++){
        //Create the thread and pass the filename
        err = pthread_create(&threads[i], NULL, eachThread, (void *)file[i]);
        if (err){
          err_sys("ERROR; return code from pthread_create() is %d\n", err);
        }
        //Make the main wait for the threads to finish
        pthread_join(threads[i], NULL);
   }

   //---------------------------------------------------------------------------
   //Print to the file
   //Merge all the files
   FILE *output = fopen(outputFile, "w+");
   if(output == NULL){
     err_sys("Error opening the file to write to");
   }
   record_t *lowest;
   int startFile = 0;
   int foundStart = 0;
   int lowestInt = -1;
   for(int i = 0; i < totalRecords; i++){
     //find the lowest record
     //Increment the starting position because all the records have been used
     foundStart = 0;
     //Increment the start position until an array with un merged items
     while(foundStart == 0){
       if(recordCounter[startFile] >= recordLength[startFile]){
         startFile++;
        }
      else {
        foundStart = 1;
        }
      }
      //Save the first available record as the lowest
     lowest = &sortedFiles[startFile][recordCounter[startFile]];
     for(int j = startFile; j < num_files; j++){
       //Is the array in bounds
       if(recordCounter[j] < recordLength[j]){
          //Compare to the current lowest record
            if(lowest -> index > sortedFiles[j][recordCounter[j]].index){
              //Set the lowest
              lowest = &sortedFiles[j][recordCounter[j]];
              //Increment the position of the sorted array
              lowestInt = j;
            }
       }
     }//Find the current lowest records
     if(lowest == &sortedFiles[startFile][recordCounter[startFile]]){
       //Incremenet the position of the array that was taken from
       recordCounter[startFile]++;
     }
     else
      recordCounter[lowestInt]++;

     //Write the lowest record
     fprintf (output, "%s,", lowest -> username);
     fprintf (output, "%s,", lowest -> password);
     fprintf (output, "%s,", lowest -> blood);
     fprintf (output, "%s,", lowest -> domain);
     fprintf (output, "%d\n", lowest -> index);
    }

   err = fclose(output);
   if(err != 0){
     err_sys("Error closing the output file");
   }
   //Free the struct data
   for(int i =0; i < num_files; i++){
     for(int j = 0; j < recordLength[i]; j++){
       //free struct information
       free(sortedFiles[i][j].username);
       free(sortedFiles[i][j].password);
       free(sortedFiles[i][j].domain);
       free(sortedFiles[i][j].blood);
     }
     //free the struct
     free(sortedFiles[i]);
     //free the filename
     free(file[i]);
   }
   //free the array of arrays
   free(sortedFiles);

   //free the record lengths
   free(recordLength);
   free(file);

}//End of else valid arguments
  printf("Data written to - %s\n", outputFile);
  printf("\nDone\n");
  return 0;
}//End of main

//Receive the filename, open it, read data into a record array
//Sort the record array, return it
void *eachThread(void *fileName) {
  //Open the file and read it into an array of records
  //500 records at the size of a record pointer
  record_t *recordList = calloc(defaultRecords, sizeof(record_t));
  //Open the file for reading
  FILE *openFile = fopen(fileName, "r");
  //Allocate for a single line
  char *line = calloc(1, defaultLineLength);
  //Read the line
  int total = 0;
  int numberOfExtensions = 1;
  while(fgets(line, defaultLineLength, openFile)) {
    //Line to hold the broken substring
    char *tempLine = strtok(line, ",");
    record_t record;
    //Split the line
    int count = 0;
    while(tempLine != NULL){
      //Break by comma
      switch(count) {
        case 0:
          record.username = calloc(1, strlen(tempLine) + 1);
          strcpy(record.username, tempLine);
          break;
        case 1:
          record.password = calloc(1, strlen(tempLine) + 1);
          strcpy(record.password, tempLine);
          break;
        case 2:
          record.blood = calloc(1, strlen(tempLine) + 1);
          strcpy(record.blood, tempLine);
          break;
        case 3:
          record.domain = calloc(1, strlen(tempLine) + 1);
          strcpy(record.domain, tempLine);
          break;
        case 4:
            record.index = atoi(tempLine);
          break;
        default:
          break;
      }
      //Break the remaining line
      tempLine = strtok(NULL, ",");
      count++;
    }//End of creating individual record

    //Add the record to the array
    recordList[total] = record;
    //Increment the total records for this file
    total++;

    //Re allocate space per file
    if(total >= (numberOfExtensions * defaultRecords)) {
      //Extend the list
      numberOfExtensions++;
      recordList = realloc(recordList, (numberOfExtensions * defaultRecords * sizeof(record_t)));
    }
  }
  //Sort the records
  qsort(recordList, total, sizeof(record_t), compareRecords);
  //Add the sorted array to the total record list
  sortedFiles[currentRecord] = recordList;
  //Add the number of records for this file to the total
  recordLength[currentRecord] = total;
  totalRecords += total;
  currentRecord++;
  //close the file
  fclose(openFile);
  //Exit the thread
  pthread_exit(NULL);
}

void printALine(FILE *file) {
  char *line = calloc(1, 11);
  fgets(line, 10, file);
  printf("%s\n", line);
}

void printStruct(record_t record) {
  printf("%s\n", record.username);
  printf("%s\n", record.password);
  printf("%s\n", record.blood);
  printf("%s\n", record.domain);
  printf("%d\n\n", record.index);
}

int compareRecords(const void *left,const void *right) {
    record_t *left1 = (record_t *) left;
    record_t *right1 = (record_t *) right;
    int compare = left1->index - right1 ->index;
    //printf("Compre - %d", compare);
    return compare;
}
