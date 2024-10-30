# Compilateur et options
CC = gcc
CFLAGS = -Wall
TARGET = src/main

# Trouve tous les fichiers .c dans le dossier + créer liste des fichiers objets .o 
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lreadline

# Règle de compilation fichier .c en .o
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)
