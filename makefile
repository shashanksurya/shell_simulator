CC=gcc
CFLAGS= -g -Wall
TARGET=sim

all: $(TARGET)

$(TARGET):  $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
