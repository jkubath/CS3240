#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "my_malloc.h"

int main(int argc, char ** argv){

	char * p0;
	char * p1;
	char * p2;
	char * p3;
	char * p4;
	char * p5;
	char * p6;
	char * p7;
	char * p8;
	char * p9;

	p0=my_malloc(7);
	p1=my_malloc(50);
	p2=my_malloc(200);
	p3=my_malloc(4096);
	p4=my_malloc(32);
	p5=my_malloc(5);

	printf("p0 lives at %p\n",p0);
	printf("p1 lives at %p\n",p1);
	printf("p2 lives at %p\n",p2);
	printf("p3 lives at %p\n",p3);
	printf("p4 lives at %p\n",p4);
	printf("p5 lives at %p\n",p5);

	my_free(p1);
	my_free(p2);
	my_free(p4);
	my_free(p5);

	printf("p1 now lives at %p\n",p1);
	printf("p2 now lives at %p\n",p2);

	p6=my_malloc(30);
	p7=my_malloc(100);

	printf("p6 lives at %p\n",p6);
	printf("p7 lives at %p\n",p7);

	my_free(p3);

	p8=my_malloc(500);
	printf("p8 lives at %p\n",p8);

	p9=my_malloc(5000);
	printf("p9 lives at %p\n",p9);

	my_free(p6);
	my_free(p7);
	my_free(p8);
	my_free(p9);

	p0=my_malloc(4096);
	printf("p0 now lives at %p\n",p0);
	my_free(p0);
	p0=my_malloc(4096);
	printf("p0 now lives at %p\n",p0);

	return 0;
}
