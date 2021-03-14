#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include "libmonitor.h"
#define BUFFERSIZE sizeof(int)

void killHandler(){
	exit(0);
}

void ctrlC(){
	exit(0);
}

int main(int argc, char *argv[]){
	//printf("I am here.");
	signal(SIGKILL, killHandler);
	signal(SIGINT, ctrlC);
	srand(time(NULL));
	int randomTime;
	while(1){
		randomTime = (rand() % 5) + 1;
		append();
		sleep(randomTime);
	}
return 0;
//////////END OF MAIN//////////////////
}
