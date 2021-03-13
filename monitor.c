#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "libmonitor.h"

int noOfProcess = 19;		//default no. of processes is 20 - 1
int n = 6;			//default no. of consumers
int m = 2;			//default no. of producers
int timeTermination = 100;	//default time termination
char logfile[30];		//default logfile name
int *shmPtr;			//shared memory ptr
int shmid;			//shared memory id
int semid;			//shared memory id for semaphore
int logid;
char *logPtr;
int x;				//can be used for loops 
pid_t *pids;
#define BUFFERSIZE sizeof(int)
#define BUFFERLOGSIZE sizeof(char)


//Displays help menu and how to use the program
void help(){
	printf("Help Menu: \n");
	printf("USAGE: \t\t./monitor [-h] [-o logfile] [-p m] [-c n] [-t time]\n\n");
	printf("This program solves the producer consumer problem by implementing semaphores \n");
	printf("into a monitor.\n\n");
	printf("Options: \n");
	printf("[-h]        \t\t-this option shows the help menu of the program. \n");
	printf("[-o] logfile\t\t-this option allows the user to specify a logfile name with a maximum \n");
	printf("            \t\tof 30 chars. Please note that end the filename with .txt so it can be cleaned. \n");
	printf("[-p m]      \t\t-this option allows the user to provide the number of producers. \n");
	printf("            \t\tsince the defualt consumers is 6 and the consumers have to be more than \n");
	printf("            \t\tthe producers, the max value of producers is 5.\n");
	printf("[-c n]      \t\t-this option allows the user to specify the number of consumers. \n");
	printf("[-t time]   \t\t-this option allows the user to specify a time in secs to when the program\n");
	printf("            \t\twill terminate no matter what.\n");

}

//deallocates and frees everything
void cleanAll(){
	shmdt(shmPtr);
	shmctl(shmid, IPC_RMID, NULL);
	shmctl(logid, IPC_RMID, NULL);
	//need a semctl to remove semaphore
	semctl(semid, 0, IPC_RMID, NULL);
}

void ctrlC(){
	cleanAll();
	exit(0);
}

void timesUp(){
	printf("The time given is up. Process will terminate.\n");
	cleanAll();
	exit(0);
}

void initPid(){
	for(x = 0; x < noOfProcess; x++){
		pids[x] = 0;
	}
}

int checkTime(int n){
	if(n > 100){
		printf("Will default to time 100s.\n");
		return 100;
	} else {
		return n;
	}
}

void initSemaphores(){
	semctl(semid, BUFFER, SETVAL, 0);
	semctl(semid, MUTEX, SETVAL, 1);
	semctl(semid, AVAILABLE, SETVAL, 4);
}

int checkFileName(int n){
	int y;
	if(n > 30){
		printf("File name too long, defaulting to logfile.txt");
		return 1;
	} else {
		return 2;
	}
}

void resetPid(pid_t p){
	int i;
	for(i = 0; i < noOfProcess; i++){
		if(pids[i] == p){
			pids[i] = 0;
		}
	}
}

int freeIndex(){
	int i;
	for(i = 0; i < noOfProcess; i++){
		if(pids[i] == 0){
			return i;
		}
	}
}
//referred to stackoverflow
void mySigchldHandler(int sig){
	pid_t pid;
	int status;

	while((pid = waitpid(-1, &status, WNOHANG)) != -1){
		resetPid(pid);
	}
}

int main(int argc, char *argv[]){
	
	int a;			//for loop
	int b;			//for loop
	int c;			//used for getopt
	key_t key;		//key for shared memory
	int count = 6;	
	int z;			//logfile check
	strcpy(logfile, "logfile.txt");
	/*if(argc == 1){
		errno = 3;
		perror("monitor: Error: Please refer to -h help for proper use of the program.");
		return 0;
	}*/

	signal(SIGALRM, timesUp);
	signal(SIGINT, ctrlC);

	//referred from stackoverflow
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = mySigchldHandler;
	sigaction(SIGCHLD, &sa, NULL);

	alarm(timeTermination);			//default time
	//get option
	while((c = getopt(argc, argv, "ho:p:c:t:")) != -1){
		switch(c){
			//displays help menu
			case 'h':
				help();
				return 0;
				break;
			
			//NOT DONE
			case 'o':
				z = strlen(optarg);
				if(z < 30){
					//printf("hi\n", m);
					strcpy(logfile, optarg);
		
				}else{
					//printf("deez %d\n", m);
					strcpy(logfile, "logfile.txt");	
				}

				break;

			//sets the no. of producers
			case 'p':
				m = atoi(optarg);
				printf("Number of producers: %d\n", m);
				break;

			//sets the no. of consumers
			case 'c':
				n = atoi(optarg);
				printf("Number of Consumers: %d\n", n);
				break;

			//sets the time of termination no matter what
			case 't':
				timeTermination = checkTime(atoi(optarg));
				printf("Process will terminate in %d sec(s)\n", timeTermination);
				alarm(timeTermination);
				break;

			default:
				errno = 3;
				perror("Please refer to -h help for proper use of the porgram");
				exit(0);
		}
	}
	///////////END of get option
	
	if(m > 6){
		printf("Sorry there should be more consumers than producers.\n");
		printf("Since the consumer default is 6, producers will be set to 5.\n");
		m = 5;
	}
	if(n < 2){
		printf("Sorry there should be more consumers than producers.\n");
		printf("Since the producer default is 2, consumers will be set to 1.\n");
		n = 1;
	}
	
	//Allocate shared memory that holds the file name
	key_t logKey = ftok("Makefile" , 'a');

	logid = shmget(logKey, BUFFERLOGSIZE * 40, IPC_CREAT | 0666);
	if(logid < 0){
		perror("monitor: Error: shmget error, creation failure");
		cleanAll();
		exit(1);
	}
	//attach to shared memory
	logPtr = (char *)shmat(logid, NULL, 0);
	if(logPtr == (char *) -1){
		perror("monitor: Error: shmat error, attachment failure");
		cleanAll();
		exit(1);
	}

	sprintf(logPtr, "%s", logfile);
	
	//init PIDs
	pids = malloc (sizeof(pid_t) * noOfProcess);
	initPid();
		
	//Allocate shared memory for the buffer
	key_t semKey;
	key = ftok("./README.md", 'a');

	shmid = shmget(key, BUFFERSIZE * count, IPC_CREAT | 0666);
	if(shmid < 0){
		perror("monitor: Error: shmget error, creation failure");
		cleanAll();
		exit(1);
	}

	//attach to shared memory
	shmPtr = (int *)shmat(shmid, NULL, 0);
	if(shmPtr == (int *) -1){
		perror("monitor: Error: shmat error, attachment failure");
		exit(1);
	}

	//allocate shared memory semaphore
	semKey = ftok("./producer.c",'a');
	semid = semget(semKey, 4, IPC_CREAT | 0666);
	initSemaphores();

	
	printf("In monitor the mutex value is: %d\n", semctl(semid, MUTEX, GETVAL, NULL));
	shmPtr[NEXTIN] = 0;
	shmPtr[NEXTOUT] = 0;

	
	/*/TEST

	pids[1] = fork();
		if(pids[1] == 0){
			
			execl("./producer", "./producer", (char *)0);
		}
	pids[2] = fork();
		sleep(1);
		if(pids[2] == 0){
			execl("./producer", "./producer", (char *)0);
		}
		
	pids[2] = fork();
		if(pids[2] == 0){
			
			execl("./consumer", "./consumer", (char *)0);
		}
	pids[3] = fork();
		if(pids[3] == 0){
			
			execl("./consumer", "./consumer", (char *)0);
		}

	pids[4] = fork();
		if(pids[4] == 0){
			
			execl("./consumer", "./consumer", (char *)0);
		}*/

	
	int pidIndex;
	srand(time(NULL));
	//producers
	for(a = 0; a < m; a++){
		//sleep(rand()%3+1);
		pidIndex = freeIndex();
		pids[pidIndex] = fork();
		if(pids[pidIndex] == 0){
			execl("./producer", "./producer", (char *)0);
		}
	}

	//consumers
	for(b = 0; b < n; b++){
		pidIndex = freeIndex();
		pids[pidIndex] = fork();
		if(pids[pidIndex] == 0){
			execl("./consumer", "./consumer", (char *)0);
		}
	}

	//Finish, track the consumers and terminate when all consumers are done
	sleep(25);
	cleanAll();

	return 0;

////////////////////////////END OF MAIN//////////////////////////////////////////
}
