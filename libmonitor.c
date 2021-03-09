#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <time.h>

#define BUFFER sizeof(int)

int nextin, nextout;
int count = 0;
int N = 4;

sem_t notfull, notempty;

int shmid;
int *shmPtr;


void append (int x){
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

	if(count == 0){
		//sem_wait(&notfull);
	printf("waiting\n");
		wait(&notfull);
	} 
	shmPtr[nextout] = x;
	nextin = (nextin + 1) % N;
	count++;

	//sem_signal(&notempty);
	signal(notempty);

	printf("Appending: %d\n", shmPtr[nextout]);

	printf("Count: %d\n", count);
}
