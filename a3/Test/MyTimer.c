#include <unistd.h>
#include <stdio.h>
#include "apue.h"

int main(int argc, char *argv[]) {
  int total = atoi(argv[1]);
  int interval = atoi(argv[2]);
  for (int i = total; i >= 0; i--) {
    if (!i) {
      printf("TIME IS UP!!!\n");
      break;
    } else if ((total - i) % interval == 0) {
      printf("%d seconds remaining.\n", i);
    }
    sleep(1);
  }
  return 0;
}
