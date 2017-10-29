#include <stdio.h>
#include "apue.h"
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

int makeargv(char *, char *, char ***);
int changeSTDIn(char **ExecVectors);
int changeSTDOut(char **ExecVectors, int position);
int convertUmask(int number);

int main(int argc, char const *argv[]) {
    //Disable CTRL C
    sigset_t sigs;
    sigemptyset(&sigs);
    sigaddset(&sigs, SIGINT);
    sigprocmask(SIG_BLOCK, &sigs, 0);

    //Number returned from makeargv
    int count = 0;
    int cmdCount = 0;
    int totalCMD = 1;
  	char ** ExecVectors;
    char *line = calloc(1, 1024);
    pid_t pid;
    int forceCont = 0;

    printf("blueShell. Type quit to exit\n");
  	while (1) {
      totalCMD = 1;
      cmdCount = 0;
  		/* Print the command prompt */
  		printf("$ ");
  		fflush(NULL);

  		/* Read a command line */
  		if (!fgets(line, 1024, stdin))
  			return 0;

      count = makeargv(line, "\t \n", &ExecVectors);

  		int input = 0;

      //Print the arguments given
      for (int i = 0; i < count; i++){
    		fprintf(stderr, "%s ", ExecVectors[i]); //stderr finds its way to
    		//console anyway
        if(strcmp(ExecVectors[i], "|") == 0){
          totalCMD++;
          ExecVectors[i] = NULL;
        }
      }
      if(strcmp(ExecVectors[count - 1], "&")){
        forceCont = 1;
      }
      printf("\n");

      //------------------------------------------------------
      int tempCount = 0;
      //Run through the line
  		while (tempCount < count) {
        //Exit command
        if(strcmp(ExecVectors[tempCount], "quit") == 0){
          free(line);
          return 0;
        }
        else if(strcmp(ExecVectors[0], "cd") == 0){
          //call the local function
          if(chdir(ExecVectors[1]) == -1){
            err_ret("Error changing directory");
          }
        }
        else if(strcmp(ExecVectors[0], "umask") == 0){
          //In the umask function
          char *directory = calloc(1, 2048);
          getcwd(directory, 2048);
          struct stat fileStat;
          //Get the information held in the directory
          if(lstat(directory, &fileStat) == -1){
            printf("Error retrieving %s information\n", directory);
          }

          //Just run umask
          if(ExecVectors[1] == NULL){
            long perm = 0;
            //printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
            //256 128 96 48 24 12 6 3 1
            (fileStat.st_mode & S_IRUSR) ? perm = perm + 4: perm;
            (fileStat.st_mode & S_IWUSR) ? perm = perm + 2: perm;
            (fileStat.st_mode & S_IXUSR) ? perm = perm + 1: perm;
            perm = perm * 10;
            (fileStat.st_mode & S_IRGRP) ? perm = perm + 4: perm;
            (fileStat.st_mode & S_IWGRP) ? perm = perm + 2: perm;
            (fileStat.st_mode & S_IXGRP) ? perm = perm + 1: perm;
             perm = perm * 10;
            (fileStat.st_mode & S_IROTH) ? perm = perm + 4: perm;
            (fileStat.st_mode & S_IWOTH) ? perm = perm + 2: perm;
            (fileStat.st_mode & S_IXOTH) ? perm = perm + 1: perm;
            perm = 777 - perm;
            printf("0%03ld\n\n", perm);
          }
          else {
            char *ptr;
            int number = strtol(ExecVectors[1], &ptr, 10);
            //A number is also given
            if(number > 777){
              printf("Enter a number less than 777\n");
            }
            else if(number / 100 > 7 || (number / 10) % 10 > 7 || (number % 10) > 7)
              printf("Enter digits less than 7\n");
            else
              //Run umask
              convertUmask(number);

            free(directory);
          }



        }
        //One command is given
        else if(totalCMD == 1){

          cmdCount++;
          pid = fork();

          if(pid == 0){
            int position = 0;
            if((position = changeSTDIn(ExecVectors)) != 0){
              //Open the file to read from
              ExecVectors[position] = NULL;
              int inputFile = open(ExecVectors[position + 1], O_RDONLY);
              if(inputFile == -1){
                err_sys("Error opening file to read");
              }
              dup2(inputFile, STDIN_FILENO);
            }

            int last = 0;
            if((last = changeSTDOut(ExecVectors, position + 1)) != 0){
              //Open the file to read from
              ExecVectors[last] = NULL;
              int outputFile = open(ExecVectors[last + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
              if(outputFile == -1){
                err_sys("Error opening file to write to");
              }
              dup2(outputFile, STDOUT_FILENO);
            }
            if(execvp( ExecVectors[0], &ExecVectors[0]) == -1){
                //Failed to run the command
                err_ret("Command not found");
            }
          }
        }
        else
        {
          //Keep track of how many commands will be run
          cmdCount++;
          int pipeData[2];

          //Pipe over the input and output
          pipe(pipeData);
          //Fork the process
          pid_t pid = fork();

          //Child process
          if(pid == 0){
            //First command
            if(cmdCount == 1){
              input = 0;
              //The < is found
              int position = 0;
              if((position = changeSTDIn(ExecVectors)) != 0){
                //Open the file to read from
                int inputFile = open(ExecVectors[position + 1], O_RDONLY);
                if(inputFile == -1){
                  err_sys("Error opening file");
                }
                dup2(inputFile, STDIN_FILENO);
              }
              //Duplicate the STDOUT_FILENO to the write of the pipe
              dup2(pipeData[1], STDOUT_FILENO);
            }


            //Middle commands
            else if(cmdCount > 1 && cmdCount < totalCMD){
              //Duplicate the stdin
              dup2(input, STDIN_FILENO);
              dup2(pipeData[1], STDOUT_FILENO);
            }
            else{
              //Last command
              int last = 0;
              if((last = changeSTDOut(ExecVectors, tempCount + 1)) != 0){
                //printf("Found > \n");
                //Open the file to read from
                ExecVectors[last] = NULL;
                int outputFile = open(ExecVectors[last + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
                if(outputFile == -1){
                  err_sys("Error opening file to write to");
                }
                dup2(input, STDIN_FILENO);
                dup2(outputFile, STDOUT_FILENO);
              }
              else
                dup2(input, STDIN_FILENO);
              //dup2(pipeData[1], STDOUT_FILENO);
            }

            if(execvp( ExecVectors[tempCount], &ExecVectors[tempCount]) == -1){
                //Failed to run the command
                err_sys("Command not found");
            }
          }

          if(input != 0){
            close(input);
          }

          //Close the write pipe
          close(pipeData[1]);

          if(cmdCount == totalCMD){
            close(pipeData[0]);
          }

          input = pipeData[0];
        }

        //Increment to the null
        while(ExecVectors[tempCount] != NULL){
    			tempCount++;
    		}
        //Add one to start at the next command
    		tempCount++;
  		}//End of while loop for executing commands
      int status = 0;
      if(forceCont != 0){
      for(int i = 0; i < cmdCount; i++){
        waitpid(pid, &status, 0);
      }
    }

  	}//End of while Get commands

  return 0;
} //End of main

int changeSTDIn(char **ExecVectors){
  int current = 0;
  //See if the first command has piped in data for std in
  while(ExecVectors[current] != NULL){
    if(strcmp(ExecVectors[current], "<") == 0){
      //< is found
      return current;
    }
    current++;
  }
  //< is not found
  return 0;
}

int changeSTDOut(char **ExecVectors, int position){
  int current = position;
  //See if the first command has piped in data for std in
  while(ExecVectors[current] != NULL){
    if(strcmp(ExecVectors[current], ">") == 0){
      //< is found
      return current;
    }
    current++;
  }
  //< is not found
  return 0;
}

int convertUmask(int number){
  printf("Number - %d\n", number);
  int n1 = number / 100 * 64;
  int n2 = (number / 10) % 10 * 8;
  int n3 = number % 10;
  int converted = n1 + n2 + n3;

  return umask(converted);
}
