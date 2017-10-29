all:
	gcc -Wall -Wpedantic -std=c99 -g BuildDataBase.c error.c -o BuildDataBase
	gcc -Wall -Wpedantic -std=c99 -g UseDataBase.c error.c -o UseDataBase
build:
	./BuildDataBase
use:
	./UseDataBase < test-input.txt
