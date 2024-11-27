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
                args[com_cont] = strndup(start, current - start);
                if (!args[com_cont]){
                    perror("erreur allocation mémoire");
                    goto cleanup;
                }
                com_cont++;
                if (com_cont >= MAX_COM - 1) break; // Empêcher un débordement
            }
            start = current + 1; // Passer au prochain mot
        }
        // Si on rencontre une accolade ouvrante
        else if (*current == '{') {
            if (in_braces) {
                fprintf(stderr, "Erreur : accolade ouvrante supplémentaire détectée.\n");
                goto cleanup;
            }
            in_braces = 1;
            start = current + 1;
        }
        // Si on rencontre une accolade fermante
        else if (*current == '}') {
            if (!in_braces) {
                fprintf(stderr, "Erreur : accolade fermante sans ouverture détectée.\n");
                goto cleanup;
            }
            in_braces = 0;
            args[com_cont] = strndup(start, current - start);
            if (!args[com_cont]){
                perror("erreur allocation mémoire");
                goto cleanup;
            }
            com_cont++;
            if (com_cont >= MAX_COM - 1) break;
            start = current + 1;
        }
        current++;
    }

    // Ajouter le dernier argument s'il y en a un
    if (current > start) {
        args[com_cont] = strndup(start, current - start);
        if (!args[com_cont]){
            perror("erreur allocation mémoire");
            goto cleanup;
        }
        com_cont++;
    }

    args[com_cont] = NULL; // Terminer le tableau d'arguments avec NULL
    return args;

    cleanup : 
    for (int i = 0; i < com_cont; i++) {
        free(args[i]);
    }
    free(args);
    exit(EXIT_FAILURE);
}

/*char **decoupe(char *command1) {
    char **args = malloc(MAX_COM * sizeof(char *));
    if (!args) {
        perror("Erreur d'allocation mémoire pour args");
        exit(EXIT_FAILURE);
    }

    int com_cont = 0; // Nombre d'éléments dans la commande
    int in_braces = 0; // Indicateur pour savoir si nous sommes dans des accolades
    int after_if = 0;  // Indicateur pour savoir si nous traitons le "if ... {"
    char *start = command1;  // Pointeur pour marquer le début de chaque mot ou chaîne
    char *current = command1;

    while (*current != '\0') {
        // Si nous rencontrons un espace ou un retour à la ligne
        if (isspace(*current) && !in_braces && !after_if) {
            if (current > start) {
                args[com_cont] = strndup(start, current - start);
                if (!args[com_cont]) {
                    perror("Erreur allocation mémoire");
                    goto cleanup;
                }
                com_cont++;
                if (com_cont >= MAX_COM - 1) break; // Empêcher un débordement
            }
            start = current + 1; // Passer au prochain mot
        }
        // Détecter "if" pour commencer à capturer tout jusqu'à la première accolade
        else if (strncmp(current, "if", 2) == 0 && (current == command1 || isspace(*(current - 1)))) {
            args[com_cont] = strndup("if", 2);
            if (!args[com_cont]) {
                perror("Erreur allocation mémoire");
                goto cleanup;
            }
            com_cont++;
            current += 2; // Passer après "if"
            start = current;
            after_if = 1; // Activer le mode capture après "if"
        }
        // Si on rencontre une accolade ouvrante après "if", on capture tout entre "if" et "{"
        else if (*current == '{' && after_if) {
            args[com_cont] = strndup(start, current - start);
            if (!args[com_cont]) {
                perror("Erreur allocation mémoire");
                goto cleanup;
            }
            com_cont++;
            if (com_cont >= MAX_COM - 1) break; // Empêcher un débordement
            start = current + 1; // Passer après l'accolade ouvrante
            after_if = 0; // Désactiver le mode spécial après "if"
            in_braces = 1; // Entrer dans une accolade
        }
        // Si on rencontre une accolade ouvrante (cas général)
        else if (*current == '{') {
            if (in_braces) {
                fprintf(stderr, "Erreur : accolade ouvrante supplémentaire détectée.\n");
                goto cleanup;
            }
            in_braces = 1;
            start = current + 1; // Commencer après l'accolade
        }
        // Si on rencontre une accolade fermante
        else if (*current == '}') {
            if (!in_braces) {
                fprintf(stderr, "Erreur : accolade fermante sans ouverture détectée.\n");
                goto cleanup;
            }
            in_braces = 0;
            args[com_cont] = strndup(start, current - start); // Capturer le contenu entre les accolades
            if (!args[com_cont]) {
                perror("Erreur allocation mémoire");
                goto cleanup;
            }
            com_cont++;
            if (com_cont >= MAX_COM - 1) break;
            start = current + 1; // Passer après l'accolade fermante
        }
        current++;
    }

    // Ajouter le dernier argument s'il y en a un
    if (current > start) {
        args[com_cont] = strndup(start, current - start);
        if (!args[com_cont]) {
            perror("Erreur allocation mémoire");
            goto cleanup;
        }
        com_cont++;
    }

    args[com_cont] = NULL; // Terminer le tableau d'arguments avec NULL
    return args;

cleanup:
    for (int i = 0; i < com_cont; i++) {
        free(args[i]);
    }
    free(args);
    exit(EXIT_FAILURE);
}*/

