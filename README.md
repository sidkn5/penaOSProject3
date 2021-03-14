Student: Sean Dela Pena

Professor: Dr. Sanjiv Bhatia

Date: 3/13/2021

Assignment 3: The purpose of the assignment is to solve the producer consumer problem
	      by implementing semaphores into a monitor.

USAGE: ./monitor [-h] [-o logfile] [-p m] [-c n] [-t time]

Problems/Notes:


		The instructions were really unclear on this project. On the description it
		says to use the monitor from the dining philosopher problem but in teams it
		says that we don't really have to follow it. In this program the append and 
		take functions are based on the monitor solution for the producer consumer
		problem given in the book. The wait and signal, however, are based on the 
		dining philosophers problem given in the book. The semaphore usage and the setup
		of the wait and signal are all based on the book and the sources that I will 
		cite on monitor.c.

		The logifle that will be given using the -o option can be any string that is
		less than 30 chars. Please note that if you don't add the .txt extension it 
		will not be removed when performing a make clean. The default name is logfile.txt

		One problem I had was that I was not taking care of the death of the children process
		properly. So even though the consumers have already died my monitor program won't end
		until the alarm goes off. Also the semaphores and the monitors where really difficult
		and confusing to implement especially with using the solution given in the book.
		It was even hard to find sources online because no one seems to have a monitor
		setup using semaphores and a communication between processes. 

		In terms of bugs, I didn't really find any when I was testing.


Functions in monitor.c

help:

	-displays the help menu of the program

killChild:

	-kills all the children processes.

cleanAll:

	-cleans up everyting like freeing pointers and shared memories

ctrlC:

	-will be called when ctrl+c is given

timesUp:

	-will be called when the time given using the -t option is up

initPid:

	-initializes the pids that will be used to 0

initSemaphores:

	-initializes the semaphores that will be used in this program.

checkFileName:

	-checks whether the given file name is too long, default to logfile.txt

resetPid:

	-resets the pids

freeIndex:

	-looks for a free space in pids, and returns that index to be used by the pid

mySigchldHandler:

	-handles the death of a process and calls the resetPid to reset that pid to 0/free

main:

	-main driver


Functions in libmonitor.c


loggingProducer:

	-logs the random item that is produced and at what time the item is produced

loggingConsumer:

	-logs the item that is consumed and at what time the item is consumed

sem_wait:

	-decrements the semaphore, this function is based on the dining philosopher's problem

sem_signal:

	-increments the semaphore, this function is based on the dining philosopher's problem

produceRandomItem:

	-this function is called by the append function to produce a random item

handler:

	-this function frees the shared memory ptrs used

append:

	-called by the producer to append a random item in the buffer, based on the monitor
	 solution given in the book.

take:

	-called by the consumer to consume an item in the buffer, based on the monitor
	 solution given in the book.
