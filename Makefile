CC = gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
EXEEXT :=
ifeq ($(OS),Windows_NT)
EXEEXT := .exe
endif
TARGET := logic-sim$(EXEEXT)
SRCS := main.c circuit.c gate.c pin.c
OBJS := $(SRCS:.c=.o)
HEADERS := circuit.h gate.h pin.h

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)
