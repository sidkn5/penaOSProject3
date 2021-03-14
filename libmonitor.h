#define NEXTIN 4	
#define NEXTOUT 5
#define	BUFFER 1
#define AVAILABLE 2
#define MUTEX 0
#define CONSUMERTRACKER 3
#define SPOTAVAILABLE 4

void append();
void take();
void sem_wait();
void sem_signal();
void loggingProducer(int);
void logginConsumer(int);
