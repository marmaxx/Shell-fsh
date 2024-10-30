# Compilateur et options
CC = gcc
CFLAGS = -Wall
TARGET = main

# Trouve tous les fichiers .c dans le dossier + créer liste des fichiers objets .o 
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lreadline

# Règle de compilation fichier .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGET)
