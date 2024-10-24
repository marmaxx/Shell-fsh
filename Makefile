CC = gcc
CFLAGS = -Wall
TARGET = main


all: $(TARGET)

$(TARGET): $(TARGET).o 
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).o  -lreadline

$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) -c $(TARGET).c

clean:
	rm -f *.o $(TARGET)
