#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>

int n = 6;			//default no. of consumers
int m = 2;			//default no. of producers
int timeTermination = 100;	//default time termination
FILE logfile;			//default logfile name
int *shmPtr;
int shmid;
#define BUFFER sizeof(int)

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
	int c;			//used for getopt
	key_t key;
	int count = 16;	
	
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

	cleanAll();

	return 0;

////////////////////////////END OF MAIN//////////////////////////////////////////
}
