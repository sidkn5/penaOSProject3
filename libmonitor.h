#define NEXTIN 4	
#define NEXTOUT 5
#define	BUFFER 2
#define AVAILABLE 1
#define MUTEX 0

void append();
void take();
void sem_wait();
void sem_signal();
void loggingProducer(int);
void logginConsumer(int);
