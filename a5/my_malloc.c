/*
	Jonah Kubath
	CS3240
	11/28/2017
	a5

  SUMMARY:
    Personal malloc and free methods.  My_Malloc.c will allocate the requested
    space and My_free will allow that space to be used by future My_Malloc
    requests.
*/

#include <stddef.h>
#include <unistd.h>
#include "apue.h"
#include "my_malloc.h"

struct data_t {
  void *dataSize;
  int size;
  struct data_t *next;
  struct data_t *previous;
  //1 - block is in use
  //0 - block is free to use
  int isFree;
};

void *findPosition(struct data_t *begin, size_t size);

struct data_t *begin = NULL;
int firstRun = 0;
int debug = 0;
void * my_malloc(size_t size) {
  void *temp = NULL;
  //First run
  //Hold the end of the heap
  if(firstRun == 0){
    if(debug){
      printf("First run\n");
    }
    //Move the heap for the pointer
    temp = sbrk(sizeof(struct data_t));
    if(debug){
      printf("Head Node Size - %p\n", sbrk(0));
    }
    if(temp == (void *)-1){
      err_sys("Error allocating space");
    }
    begin = (struct data_t *) temp;
    //Initialize the head node
    begin -> dataSize = NULL;
    begin -> size = 0;
    begin -> next = NULL;
    begin -> previous = NULL;
    begin -> isFree = 1;

    //Create the first struct
    temp = sbrk(sizeof(struct data_t));
    if(temp ==(void *) -1){
      err_sys("Error allocating space");
    }
    struct data_t *first = (struct data_t *) temp;
    first -> previous = begin;
    first -> next = NULL;
    first -> size = size + sizeof(struct data_t);
    first -> isFree = 1;

    begin -> next = first;

    //Move the heap pointer
    first -> dataSize = sbrk(size);
    if(first -> dataSize ==(void *) -1){
      err_sys("Error allocating space");
    }
    //Turn off the first run
    firstRun = 1;
    if(debug){
      //printf("Looking for pointer to free\n");
      printf("Done making the first node\n");
    }
    return first -> dataSize;
  }//End of first run
  else {
    //Find a free pointer
    void *pointer = findPosition(begin, size);
    return pointer;
  }

}

void my_free(void * pointer) {
  if(debug){
    //printf("Looking for pointer to free\n");
    printf("Pointer to free - %p\n", pointer);
  }
  struct data_t *currentPosition = begin -> next;
  //Search the list for the pointer
  while(currentPosition != NULL){
    if(debug){
      //printf("Looking for pointer to free\n");
      printf("Current data - %p\n", currentPosition -> dataSize);
      if(currentPosition -> next != NULL)
        printf("Next data - %p\n", currentPosition -> next -> dataSize);
    }
    //Found the pointer to delete
    if(currentPosition -> dataSize == pointer){
      break;
    }
    else
      currentPosition = currentPosition -> next;
  }

  //Position not found
  if(currentPosition == NULL)
    return;

  //printf("Found - %p  %d\n", currentPosition -> dataSize, currentPosition -> isFree);
  //printf("Previous - %p  %d\n", (currentPosition -> previous) -> dataSize, (currentPosition -> previous) -> isFree);
  // if(currentPosition -> next != NULL)
  //   printf("Next - %p  %d\n", (currentPosition -> next) -> dataSize, (currentPosition -> next) -> isFree);
  //The previous node is free and is not the beginning node
  if(currentPosition != NULL && (currentPosition -> previous) -> isFree != 1){
    if(currentPosition -> previous != NULL && currentPosition -> previous != begin) {
      if(debug) {
        printf("Previous Node was free\n");
      }
      (currentPosition -> previous) -> next = currentPosition -> next;
      if(currentPosition -> next != NULL)
        (currentPosition -> next) -> previous = currentPosition -> previous;
      (currentPosition -> previous) -> size += currentPosition -> size;
      //currentPosition -> dataSize = NULL;
      currentPosition -> size = 0;
      //currentPosition -> previous = NULL;
      //currentPosition -> next = NULL;
      currentPosition = currentPosition -> previous;
      if(debug) {
        printf("Done combining previous node\n");
      }
    }
  }

  //printf("\nPrevious - %p  %d\n", (currentPosition -> previous) -> dataSize, (currentPosition -> previous) -> isFree);
  // if(currentPosition -> next != NULL)
  //   printf("Next - %p  %d\n", (currentPosition -> next) -> dataSize, (currentPosition -> next) -> isFree);
  //The next node is free and is not the end node
  if(currentPosition != NULL && currentPosition -> next != NULL){
    if((currentPosition -> next) -> isFree != 1) {
      if(debug) {
        printf("Next Node was free\n");
      }
      (currentPosition -> previous) -> next = currentPosition -> next;
      (currentPosition -> next) -> previous = currentPosition -> previous;
      (currentPosition -> previous) -> size += currentPosition -> size;
      //currentPosition -> dataSize = NULL;
      currentPosition -> size = 0;
      currentPosition -> previous = NULL;
      currentPosition -> next = NULL;
      //currentPosition = NULL;
    }
  }


  if(currentPosition != NULL) {
    //printf("Neither are free\n");
    currentPosition -> isFree = 0;
    //currentPosition -> dataSize = NULL;
  }
  else
    currentPosition = currentPosition -> previous;
  if(debug){
    printf("New Previous - %p\n", currentPosition -> dataSize);
    if(currentPosition -> next != NULL)
      printf("New Next - %p\n", currentPosition -> next -> dataSize);
    //printf("Looking for pointer to free\n");
    printf("Free is done\n\n");
  }
}

void *findPosition(struct data_t *begin, size_t size) {
  struct data_t *previousPosition = begin;
  struct data_t *currentPosition = begin -> next;
  //Iterate through the list to find an available location
  while(currentPosition != NULL){
    //printf("Current Position to find space - %p\n", currentPosition -> dataSize);
    //Current position is available to be written to
    //Current position has enough space allocated
    if(currentPosition -> isFree != 1 && currentPosition -> size >= ((int) size + sizeof(struct data_t))){
      if(debug){
        printf("Re used old allocation - %p\n", currentPosition -> dataSize);
      }
      
      //Start at the struct, move past the struct data, move past the allocated space
      struct data_t *splitNode = currentPosition + size + sizeof(struct data_t);
      //printf("New node - %p\n", splitNode);
      splitNode -> previous = currentPosition;
      splitNode -> dataSize = splitNode + sizeof(struct data_t);
      //printf("New Data - %p\n", splitNode -> dataSize);
      splitNode -> next = currentPosition -> next;
      splitNode -> size = currentPosition -> size - size - sizeof(struct data_t);
      splitNode -> isFree = 0;
      currentPosition -> next = splitNode;
      currentPosition -> size = size + sizeof(struct data_t);
      currentPosition -> isFree = 1;
      // printf("Current Allocation - %p\n", currentPosition -> dataSize);\
      // printf("New Next Allocation- %p\n", currentPosition -> next -> dataSize);
      return currentPosition -> dataSize;
    }
    else {
      previousPosition = currentPosition;
      currentPosition = currentPosition -> next;

    }
  }

  if(debug){
    printf("Moving heap pointer\n");
  }
  //If we are this far, we have not found a previously allocated block to use
  //Move the heap for the pointer
  void *newNode;
  struct data_t *node;
  newNode = sbrk(sizeof(struct data_t));
  if(newNode == (void *) -1){
    err_sys("Error allocating space");
  }
  //newNode = sbrk();
  node = (struct data_t *) newNode;
  //Initialize the head node
  node -> size = size + sizeof(struct data_t);
  node -> next = NULL;
  node -> previous = previousPosition;
  node -> isFree = 1;

  //Allocate the requested space
  newNode = sbrk(size);
  node -> dataSize = newNode;

  previousPosition -> next = node;

  return node -> dataSize;


}
