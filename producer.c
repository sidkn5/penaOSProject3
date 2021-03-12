#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>

#define BUFFERSIZE sizeof(int)

/*/this produces a random number as the "item"
int produceRandomItem(){
	int randomItem;
	randomItem = rand() % 10 + 1;

	return randomItem;
}*/

void killHandler(){
	exit(0);
}

void ctrlC(){
	exit(0);
}

int main(int argc, char *argv[]){
	printf("I am here.");
	signal(SIGKILL, killHandler);
	signal(SIGINT, ctrlC);
	int randomTime;
	srand(time(NULL));
	randomTime = (rand() % 5) + 1;
	int x = 0;
	do{
	printf("HEllo");
		append();
		sleep(randomTime);
		//sleep(1);
		x++;
	}while(1);
return 0;
//////////END OF MAIN//////////////////
}
