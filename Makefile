CC = gcc
CFLAG = -g -lm -std=c99
LIBS = -lmonitor
all: monitor producer consumer

%.o: %.c
	$(CC) $(CFLAG) -c $< -o $@

monitor: monitor.o libmonitor.a
	$(CC) -L. -o $@ monitor.o $(LIBS) $(CFLAG)

producer: producer.o libmonitor.a
	$(CC) -L. -o $@ producer.o $(LIBS) $(CFLAG)

consumer: consumer.o libmonitor.a
	$(CC) -L. -o $@ consumer.o $(LIBS) $(CFLAG)

libmonitor.o: libmonitor.c
	$(CC) -c -O $^

producer.o: producer.c
	$(CC) -c -O $^

consumer.o: consumer.c
	$(CC) -c -O $^

monitor.o: monitor.c
	$(CC) -c -O $^

libmonitor.a: libmonitor.o
	ar rcs $@ $^

clean:
	rm -f *.o monitor producer consumer *.a
