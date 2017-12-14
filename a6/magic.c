#include <stdio.h>
#include <magic.h>

int main(int argc, char * argv[]) {
  magic_t myt = magic_open(MAGIC_ERROR|MAGIC_MIME_TYPE);
  magic_load(myt,NULL);
  printf("magic output: '%s'\n",magic_file(myt,argv[1]));
  magic_close(myt);

  return 0;
}