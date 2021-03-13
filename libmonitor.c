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
	semid = semget(semKey, 4, 0);

	semaphore.sem_op = -1;
	semaphore.sem_num = n;
	semaphore.sem_flg = 0;
	semop(semid, &semaphore, 1);
}

void sem_signal(int n){

	key_t semKey = ftok("./producer.c", 'a');
	semid = semget(semKey, 4, 0);

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

	//printf("before wait In library the mutex value is: %d\n", semctl(semid, MUTEX, GETVAL, NULL));
	sem_wait(AVAILABLE);
	sem_wait(MUTEX);
//	printf("AFTER wait In library the mutex value is: %d\n", semctl(semid, MUTEX, GETVAL, NULL));
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


}


/*
void append(){
	int x;
	//use of semget
	int *vars;
	struct sembuf init_sm[] = {
		{0, BUFFER_SIZE, IPC_NOWAIT},	//initialize no of spaces in buffer
		{1, 0, IPC_NOWAIT},		//initiliaze no of elements to 0
		{2, 1, IPC_NOWAIT}		//initialize mutex = 1

	};
	
	key_t varKey = ftok("./producer",'a');
	var_id = shmget(varKey, 2 * sizeof(int), S_IRUSR | S_IWUSR);
	vars = (int *)shmat(var_id, 0, 0);
	vars[FRONT] = vars[BACK] = 0;

	//init buffer
	key_t key;
	key = ftok("./README.md", 'a');
	buffer_id = shmget(key, BUFFER_SIZE * sizeof(int), S_IRUSR | S_IWUSR);
	
	//init semaphores
	if(count > 0){

	key_t semKey = ftok("./consumer",'a');
	sem_id = shmget(semKey, 3, S_IRUSR | S_IWUSR);
	semop(sem_id, init_sm, 3);

	int i;
	char obuf[512];
	//int* vars;
	int *buffer;
	struct sembuf acquire[] = {{0, -1, 0}, {2, -1, SEM_UNDO}};
	struct sembuf release[] = {{1, 1, 0}, {2, 1, SEM_UNDO}};

	vars = (int *)shmat(var_id, 0, 0);
	buffer = (int *)shmat(buffer_id, 0, 0);

	fprintf(stderr, "Producing now %d\n", x);

	semop(sem_id, acquire, 2);
	int produce;
	buffer[vars[FRONT]] = produce;
	vars[FRONT] = (vars[FRONT] + 1) % BUFFER_SIZE;

	semop(sem_id, release, 2);

	fprintf(stderr, "Produced %d\n", x);
	count--;
	}

}

void take(){
	int x;
	//use of semget
	int *vars;
	struct sembuf init_sm[] = {
		{0, BUFFER_SIZE, IPC_NOWAIT},	//initialize no of spaces in buffer
		{1, 0, IPC_NOWAIT},		//initiliaze no of elements to 0
		{2, 1, IPC_NOWAIT}		//initialize mutex = 1

	};
	
	key_t varKey = ftok("./producer",'a');
	var_id = shmget(varKey, 2 * sizeof(int), S_IRUSR | S_IWUSR);
	vars = (int *)shmat(var_id, 0, 0);
	vars[FRONT] = vars[BACK] = 0;

	//init buffer
	key_t key;
	key = ftok("./README.md", 'a');
	buffer_id = shmget(key, BUFFER_SIZE * sizeof(int), S_IRUSR | S_IWUSR);
	
	//init semaphores
	if(count > 0){
	key_t semKey = ftok("./producer",'a');
	sem_id = shmget(semKey, 3, S_IRUSR | S_IWUSR);
	semop(sem_id, init_sm, 3);

	int i;
	char obuf[512];
	//int* vars;
	int *buffer;
	struct sembuf acquire[] = {{0, -1, 0}, {2, -1, SEM_UNDO}};
	struct sembuf release[] = {{1, 1, 0}, {2, 1, SEM_UNDO}};

	vars = (int *)shmat(var_id, 0, 0);
	buffer = (int *)shmat(buffer_id, 0, 0);

	fprintf(stderr, "Consuming now %d\n", x);

	semop(sem_id, acquire, 2);
	int consume;
	consume = buffer[vars[BACK]];
	vars[BACK] = (vars[BACK] + 1) % BUFFER_SIZE;

	semop(sem_id, release, 2);

	fprintf(stderr, "Consumed %d\n", x);
	count--;

}
}*/
