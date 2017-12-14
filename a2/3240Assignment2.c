
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "apue.h"

//Nodes for the linked list of directories
typedef struct directory_t directory_t;
struct directory_t {
  char* currentDirectory;
  directory_t *nextDirectory;
};


int main(int argc, char** argv){
  directory_t *listPointer;
  directory_t *tail;
  directory_t *head = calloc((sizeof(directory_t)), 1);
  //Store the newly found directory
  char *tempDirectory = calloc(500, 1);
  //Store the newly found directory
  //char *tempDirectory = calloc((strlen(listPointer ->currentDirectory) + strlen(dirp -> d_name) + 1), 1);

  if(argc < 2){
    printf("Invalid arguments.\n");
    printf("Invalid arguments.\t./3240Assignment2 <directory>\n");
  }
  //Valid arguments
  else {
    //Open a new directory
    DIR *dp;
    struct stat statBuff;
    //Information from a directory
    struct dirent * dirp;
    //Allocate space for string length
    head -> currentDirectory = calloc((strlen(argv[1]) + 1), 1);
    strcpy(head -> currentDirectory, argv[1]);
    //head -> currentDirectory = argv[1];
    head -> nextDirectory = NULL;
    listPointer = head;
    tail = head;

    printf("You entered: %s\n", argv[1]);
    //Change directory to the user input
    if(chdir(argv[1]) == -1){
      printf("Error changing to %s\n", argv[1]);
    };

    //Open the directory from arguments
    dp = opendir(".");
    if(dp == NULL){
      err_sys("%s%s\n", "No read rights - ", argv[1]);
    }

    while(listPointer != NULL){
    //Get the information from the directory
    printf("\nCurrent Directory - %s\n", listPointer -> currentDirectory);
    //Read the files inside the directory
    while((dirp = readdir(dp)) != NULL){
      //Gets the information from the directory and saves it to statBuff
      if(lstat(dirp -> d_name, &statBuff) == -1){
        printf("Error retrieving %s information\n", dirp -> d_name);
      };
      //Is it a directory
      if(S_ISDIR(statBuff.st_mode)){
        if(strcmp(dirp -> d_name, ".") == 0 || strcmp(dirp -> d_name, "..") == 0){
          //Do nothing
          printf("\tDirectory - %s\n", dirp -> d_name);
        }
        else {
          //Add the directory to the list
          tail -> nextDirectory = calloc((sizeof(directory_t)), 1);
          if(tail -> nextDirectory == NULL){
            err_sys("Error allocating memory\n");
          }
          //Add move the tail pointer
          tail = tail -> nextDirectory;
          //Terminate the nextDirectory pointer for the end of the list
          tail -> nextDirectory = NULL;
          //Save the directory path to a string
          strcpy(tempDirectory, listPointer -> currentDirectory);
          strcat(tempDirectory, "/");
          strcat(tempDirectory, dirp -> d_name);
          //Save the directory string
          tail -> currentDirectory = calloc(strlen(tempDirectory) + 1, 1);
          strcpy(tail -> currentDirectory, tempDirectory);
          printf("\tDirectory - %s\n", tail -> currentDirectory);
          //Clear the directory string
          memset(tempDirectory, 0, 500);
        }
      }//End of a directory
      //Is it a regular file
      else if (S_ISREG(statBuff.st_mode)){
        if(strcmp(dirp -> d_name, ".DS_Store") == 0){
          //Do nothing
        }
        else
          printf("\t%s - %s\n", "File", dirp -> d_name);

      }//End of a file
    }//End of while for reading current directory

      //Iterate the list
      listPointer = listPointer -> nextDirectory;

      //Make sure the list pointer isn't NULL
      //Change directories
      int changeDir = 0;
      while(listPointer != NULL){
        //Check for execute rights
        if(chdir(listPointer -> currentDirectory) == -1){
          printf("No execute rights - %s\n", listPointer -> currentDirectory);
          listPointer = listPointer -> nextDirectory;
        }
        //Check for read rights
        else{
            dp = opendir(listPointer -> currentDirectory);
            if (dp == NULL){
              printf("%s%s\n", "No read rights - ", listPointer -> currentDirectory);
              listPointer = listPointer -> nextDirectory;
            }
            else
              changeDir = 1;
          }
        //Break from the while loop
        if(changeDir == 1){
          break;
        }

        } //End of changing directory

    }//End of while loop for reading every directory

}//End of else valid arguments

  //Holds the directory path
  free(tempDirectory);
  //Free data
  while(head != NULL){
    listPointer = head -> nextDirectory;
    free(head -> currentDirectory);
    free(head);
    head = listPointer;
  }
  printf("\n\nDone Printing\n");
  return 0;
}//End of main
