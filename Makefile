CC = gcc
CFLAGS = -Wall
TARGET = src/main


all: $(TARGET)
	./src/main
	
$(TARGET): $(TARGET).o 
	$(CC) $(CFLAGS) -o $(TARGET) main.o  -lreadline

$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) -c $(TARGET).c

clean:
	rm -f *.o $(TARGET)
