#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "../include/decoupeCmd.h"

#define MAX_COM 64  

char **decoupe(char *command1) {
    char **args = malloc(MAX_COM * sizeof(char *));
    if (!args) {
        perror("Erreur d'allocation mémoire pour args");
        exit(EXIT_FAILURE);
    }

    int com_cont = 0; // Nombre d'éléments dans la commande
    int in_braces = 0; // Indicateur pour savoir si nous sommes dans des accolades
    char *start = command1;  // Pointeur pour marquer le début de chaque mot ou chaîne
    char *current = command1;

    while (*current != '\0') {
        // Si nous rencontrons un espace ou un retour à la ligne, et nous ne sommes pas dans des accolades
        if (isspace(*current) && !in_braces) {
            if (current > start) {
                args[com_cont++] = strndup(start, current - start);
                if (com_cont >= MAX_COM - 1) break; // Empêcher un débordement
            }
            start = current + 1; // Passer au prochain mot
        }
        // Si on rencontre une accolade ouvrante
        else if (*current == '{') {
            if (in_braces) {
                fprintf(stderr, "Erreur : accolade ouvrante supplémentaire détectée.\n");
                exit(EXIT_FAILURE);
            }
            in_braces = 1;
            start = current + 1;
        }
        // Si on rencontre une accolade fermante
        else if (*current == '}') {
            if (!in_braces) {
                fprintf(stderr, "Erreur : accolade fermante sans ouverture détectée.\n");
                exit(EXIT_FAILURE);
            }
            in_braces = 0;
            args[com_cont++] = strndup(start, current - start);
            if (com_cont >= MAX_COM - 1) break;
            start = current + 1;
        }
        current++;
    }

    // Ajouter le dernier argument s'il y en a un
    if (current > start) {
        args[com_cont++] = strndup(start, current - start);
    }

    args[com_cont] = NULL; // Terminer le tableau d'arguments avec NULL
    return args;
}
