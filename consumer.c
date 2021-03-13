#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include "libmonitor.h"

int shmid;
int *shmPtr;
#define BUFFERSIZE sizeof(int)

void killHandler(){
	exit(0);
}

int main(int argc, char *argv[]){
	signal(SIGKILL, killHandler);
	int randomTime;
	srand(time(NULL));
	randomTime = (rand() % 10) + 1;

	
	sleep(randomTime);
	take();


	exit(0);
return 0;
///////////////END OF MAIN
}
