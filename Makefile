# Compilateur et options
CC = gcc
CFLAGS = -Wall -Iinclude
TARGET = fsh

# Liste des fichiers sources
SRCS = fsh.c $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lreadline

# Règle de compilation pour les fichiers .o dans src/ et pour fsh.c à la racine
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o *.o $(TARGET)

run:
	./fsh
