#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#define THREAD_NUM 2

int n = 6;			//default no. of consumers
int m = 2;			//default no. of producers
int timeTermination = 100;	//default time termination
FILE logfile;			//default logfile name
int *shmPtr;
int shmid;
#define BUFFER sizeof(int)


//test monitor
int N = 4;
char buffer [4];
sem_t full, empty;
int nextin = 0, nextout = 0;
int count = 0;
sem_t notfull, notempty;


void append(){
	int x = 10;
	if(count == 4)
		buffer[nextin] = x;
	nextin = (nextin + 1) % N;
	count++;

	signal(notempty);
}

void take (char x){
	if(count == 0)
		wait(notempty);
	x = buffer[nextout];
	nextout = (nextout + 1)% N;
	count--;

	signal(notfull);

}

int x;
void producer(){
	srand(time(NULL));
	while(count < N){
	x = rand() % 5;
		//produce(x);
		append(x);
		printf("appended x%d\n", x);
	}
}
void consumer(){
int i;
	//while(count  N){
	for(i = 0; i < 5; i++){
		take(x);
		//consume(x);
		printf("taken x%d\n", x);}
//	}
}

//Displays help menu and how to use the program
void help(){
	printf("Help Menu: \n");
}

//deallocates and frees everything
void cleanAll(){
	shmdt(shmPtr);
	shmctl(shmid, IPC_RMID, NULL);
}

int main(int argc, char *argv[]){

producer();
consumer();
/*pthread_t th[2];
int i;

for(i = 0; i < THREAD_NUM; i++){
	if(i % 2 == 0){
		if(pthread_create(&th[i], NULL, producer, NULL) != 0){
			perror("failed crete");
		}
	}else {
		if(pthread_create(&th[i], NULL, consumer, NULL) != 0){
			perror("failed crete");
		}

	}
}

for(i = 0; i < THREAD_NUM; i++){
	if(pthread_join(th[i],NULL) != 0){
		perror("failed join");
	}
}
*/
/*	int c;			//used for getopt
	key_t key;
	int count = 4;	
	
	if(argc == 1){
		errno = 3;
		perror("monitor: Error: Please refer to -h help for proper use of the program.");
		return 0;
	}



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
				help();
				return 0;
				break;

			//sets the no. of producers
			case 'p':
				m = atoi(optarg);
				printf("Producers: %d\n", m);
				break;

			//sets the no. of consumers
			case 'c':
				n = atoi(optarg);
				printf("Consumers: %d\n", n);
				break;

			//sets the time of termination no matter what
			case 't':
				timeTermination = atoi(optarg);
				printf("Process will terminate in %d sec(s)\n", timeTermination);
				return 0;
				break;

			default:
				errno = 3;
				perror("Please refer to -h help for proper use of the porgram");
				exit(0);
		}
	}

	///////////END of get option
	
	
	
	//Allocate shared memory


	key = ftok("./README.md", 'a');

	shmid = shmget(key, BUFFER * count, IPC_CREAT | 0666);
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

	//cleanAll();

	return 0;
*/
////////////////////////////END OF MAIN//////////////////////////////////////////
}
