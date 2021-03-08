#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>

#define BUFFER sizeof(int)
int main(int argc, char *argv[]){
	
	int shmid;
	int *shmPtr;
	int randomTime= 0;
	int x;
	int buffer[4];
	int index = 0;
	int count = 4;
	//random time generate from 1-5
	srand(time(NULL));
	randomTime = rand() % 5 + 1;

	printf("Sleeping \n");
	//sleep(randomTime);
	printf("Done sleepting \n");


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


	while(index < 4){
		if(index < 4){
			x = rand() % 10;
			shmPtr[index] = x;
			index++;
		}
	}
	
	int y;
	for(y = 0; y < 4; y++){
		printf("Put in the buffer %d\n", shmPtr[y]);
	}

//////////END OF MAIN//////////////////
}
