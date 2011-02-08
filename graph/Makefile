CC = gcc
LD = gcc

CFLAGS = -g -Wall -D_REENTRANT
LDFLAGS = -lm -lX11 -lpthread

RM = /bin/rm -f
TARGET = graph
OBJ = graph.o func.o

all:	$(TARGET)
clean:
	$(RM) $(OBJ) $(TARGET)

$(TARGET): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<
