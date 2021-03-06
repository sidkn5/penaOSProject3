#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

int item = 0;

int main(int argc, char *argv[]){

	int randomTime= 0;
	//random time generate from 1-5
	srand(time(NULL));
	randomTime = rand() % 5 + 1;

	printf("Sleeping \n");
	sleep(randomTime);
	printf("DOne sleepting \n");


//////////END OF MAIN//////////////////
}
