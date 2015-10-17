CC=gcc
CFLAGS= -g -Wall

all: sim myls

sim: sim.c
	$(CC) $(CFLAGS) -o sim sim.c

myls: myls.c
	$(CC) $(CFLAGS) -o myls myls.c

clean:
	rm myls sim
