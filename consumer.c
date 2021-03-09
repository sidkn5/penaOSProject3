#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include "libmonitor.h"

int shmid;
int *shmPtr;
#define BUFFER sizeof(int)

int main(int argc, char *argv[]){

//test(5);
	int count = 4;
	int index=0;
	//Allocate shared memory
	key_t key;
	key = ftok("./README.md", 'a');

	shmid = shmget(key, BUFFER * count, IPC_EXCL);
	if(shmid < 0){
		perror("monitor: Error: shmget error, creation failure");
		//replace with clean up
		exit(1);
	}

	//attach to shared memory
	shmPtr = (int *)shmat(shmid, NULL, 0);
	if(shmPtr == (int *) -1){
		perror("monitor: Error: shmat error, attachment failure");
		exit(1);
	}
	
	int i;
	for(i=0; i < 4; i++){
		printf("Taken %d\n",shmPtr[i]);
	}

return 0;
///////////////END OF MAIN
}
