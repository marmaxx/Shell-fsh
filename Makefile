# Compilateur et options
CC = gcc
CFLAGS = -Wall -Iinclude
TARGET = fsh

# Répertoire des sources et objets
SRC_DIR = src
BUILD_DIR = build

# Liste des fichiers sources
SRCS = bin/fsh.c $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET)

# Création de l'exécutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lreadline

# Compilation des fichiers .c en .o dans le répertoire build/
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)  # Création du dossier build si nécessaire
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run:
	./$(TARGET)
