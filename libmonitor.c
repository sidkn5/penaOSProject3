#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include "libmonitor.h"
#define BUFFERSIZE sizeof(int)
#define BUFFERLOGSIZE sizeof(char)

/*
FROM libmonitor.h
#define MUTEX 0
#define	BUFFER 1
#define AVAILABLE 2
#define NEXTIN 4	
#define NEXTOUT 5
*/
int semid;
int var_id;
int buffer_id;

int shmid;
int *shmPtr;
int count = 4;
int logid;
char *logPtr;
char *logfile[30];

struct sembuf semaphore;
//need time
void loggingProducer(int n){

	time_t currentTime;
	struct tm *timeInfo;

	//Allocate shared memory that holds the file name
	key_t logKey = ftok("Makefile" , 'a');

	logid = shmget(logKey, BUFFERLOGSIZE * 40, IPC_EXCL);
	if(logid < 0){
		perror("monitor: Error: shmget error, creation failure");
		//cleanAll();
		exit(1);
	}
	//attach to shared memory
	logPtr = (char *)shmat(logid, NULL, 0);
	if(logPtr == (char *) -1){
		perror("monitor: Error: shmat error, attachment failure");
		//cleanAll();
		exit(1);
	}
	FILE *fp;
	fp = fopen(logPtr, "a");
	time(&currentTime);
	timeInfo = localtime(&currentTime);
	fprintf(fp,"Produced item %d\n", n);
	fprintf(fp,"The item produced is produced at time: %s\n\n", asctime(timeInfo));
	fclose(fp);
	shmdt(logPtr);
}

void loggingConsumer(int n)
{
	time_t currentTime;
	struct tm *timeInfo;

	//Allocate shared memory that holds the file name
	key_t logKey = ftok("Makefile" , 'a');

	logid = shmget(logKey, BUFFERLOGSIZE * 40, IPC_EXCL);
	if(logid < 0){
		perror("monitor: Error: shmget error, creation failure");
		//cleanAll();
		exit(1);
	}
	//attach to shared memory
	logPtr = (char *)shmat(logid, NULL, 0);
	if(logPtr == (char *) -1){
		perror("monitor: Error: shmat error, attachment failure");
		//cleanAll();
		exit(1);
	}
	FILE *fp;
	time(&currentTime);
	timeInfo = localtime(&currentTime);
	fp = fopen(logPtr, "a");
	fprintf(fp, "Consumer consumed item %d\n", n);
	fprintf(fp,"The item consumed is consumed at time: %s\n\n", asctime(timeInfo));
	fclose(fp);
	shmdt(logPtr);
}

void sem_wait(int n){

	key_t semKey = ftok("./producer.c", 'a');
	semid = semget(semKey, 5, 0);

	semaphore.sem_op = -1;
	semaphore.sem_num = n;
	semaphore.sem_flg = 0;
	semop(semid, &semaphore, 1);
}

void sem_signal(int n){

	key_t semKey = ftok("./producer.c", 'a');
	semid = semget(semKey, 5, 0);

	semaphore.sem_op = 1;
	semaphore.sem_num = n;
	semaphore.sem_flg = 0;
	semop(semid, &semaphore, 1);

}

int produceRandomItem(){
	int randomItem;
	srand(time(NULL));
	randomItem = rand() % 100 + 1;

	return randomItem;

}
void handler(){
	shmdt(shmPtr);
	shmdt(logPtr);
	exit(0);
}

void append (){
	int x;				//holds the random item number
	signal(SIGINT, handler);

	sem_wait(AVAILABLE);
	sem_wait(MUTEX);
	x = produceRandomItem();
	
	//Allocate shared memory
	key_t key;
	key = ftok("./README.md", 'a');

	shmid = shmget(key, BUFFERSIZE * count, IPC_EXCL);
	if(shmid < 0){
		perror("libmonitor append: Error: shmget error, creation failure");
		//replace with clean up
		exit(1);
	}

	//attach to shared memory
	shmPtr = (int *)shmat(shmid, NULL, 0);


	if(shmPtr == (int *) -1){
		perror("libmonitor append: Error: shmat error, attachment failure");
		exit(1);
	}

	shmPtr[shmPtr[NEXTIN]] = x;
	shmPtr[NEXTIN] = (shmPtr[NEXTIN] + 1) % 4;
	//count++;
	sleep(1);
	printf("Appending %d\n", x);
	loggingProducer(x);
	shmdt(shmPtr);

		/*if(semctl(semid, CONSUMERTRACKER, GETVAL, NULL) == 0){
			printf("shouldend");
			exit(0);
			}*/

	sem_signal(MUTEX);
	sem_signal(BUFFER);
}



void take(){	
	int x;				//holds the random item number
	signal(SIGINT, handler);
	sem_wait(BUFFER);
	sem_wait(MUTEX);

	//Allocate shared memory
	key_t key;
	key = ftok("./README.md", 'a');

	shmid = shmget(key, BUFFERSIZE * count, IPC_EXCL);
	if(shmid < 0){
		perror("libmonitor take: Error: shmget error, creation failure");
		//replace with clean up
		exit(1);
	}

	//attach to shared memory
	shmPtr = (int *)shmat(shmid, NULL, 0);


	if(shmPtr == (int *) -1){
		perror("libmonitor take: Error: shmat error, attachment failure");
		exit(1);
	}

	x = shmPtr[shmPtr[NEXTOUT]];
	shmPtr[NEXTOUT] = (shmPtr[NEXTOUT] + 1) % 4;
	sleep(1);

	loggingConsumer(x);
	shmdt(shmPtr);

	sem_signal(MUTEX);
	sem_signal(AVAILABLE);

	sem_wait(CONSUMERTRACKER);
	//semctl(semid, CONSUMERTRACKER, SETVAL, (semctl(semid, CONSUMERTRACKER, GETVAL, NULL)) - 1);
	printf("CONSUMER TRACKER %d\n", semctl(semid, CONSUMERTRACKER, GETVAL, NULL));

}

