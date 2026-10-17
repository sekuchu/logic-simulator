CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
TARGET = logic-sim
OBJS = main.o circuit.o gate.o pin.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

circuit.o: circuit.c
	$(CC) $(CFLAGS) -c circuit.c

gate.o: gate.c
	$(CC) $(CFLAGS) -c gate.c

pin.o: pin.c
	$(CC) $(CFLAGS) -c pin.c

clean:
	rm -f $(OBJS) $(TARGET)
