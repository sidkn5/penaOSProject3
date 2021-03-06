CC = gcc
CFLAG = -g

all: monitor producer

%.o: %.c
	$(CC) $(CFLAG) -c $< -o $@

monitor: monitor.o
	$(CC) $(CFLAG) $< -o $@ -lm

producer: producer.o
	$(CC) $(CFLAG) $< -o $@ -lm
clean:
	rm -f *.o monitor producer
