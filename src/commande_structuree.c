#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#include "../include/externe.h"
#include "../include/decoupeCmd.h"
#include "../include/fsh.h"
#include "../include/commande_structuree.h"

#define MAX_COM 64

int is_structured(const char *command) {
    return strchr(command, ';') != NULL;
}

char *trim_whitespace(char *str) {
    if (!str) return NULL;

    // Supprimer les espaces en début
    while (isspace((unsigned char)*str)) {
        str++;
    }

    // Si la chaîne est vide après suppression
    if (*str == '\0') {
        return str;
    }

    // Supprimer les espaces en fin
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        *end-- = '\0';
    }

    return str;
}

char **decoupe_commande_structuree(const char *command) {
    char **sub_commands = malloc(MAX_COM * sizeof(char *));
    if (!sub_commands) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    char *command_copy = strdup(command); // Créer une copie modifiable de la commande
    if (!command_copy) {
        perror("Erreur d'allocation mémoire pour la copie");
        free(sub_commands);
        exit(EXIT_FAILURE);
    }

    char *current = strtok(command_copy, ";");
    int cmd_count = 0;

    // Découper les sous-commandes par `;`
    while (current != NULL && cmd_count < MAX_COM - 1) {
        // Nettoyer les espaces autour de la sous-commande
        sub_commands[cmd_count++] = strdup(trim_whitespace(current));
        current = strtok(NULL, ";");
    }

    sub_commands[cmd_count] = NULL; // Terminer le tableau avec NULL

    free(command_copy); // Libérer la copie de la commande d'origine
    return sub_commands;
}

int execute_structured_command(const char *command, int last_status){
    int result;
    char **commande_decoupee = decoupe_commande_structuree(command);

    //printf("comm[0] = #%s# et comm[1] = #%s# et comm[2] = #%s#\n", commande_decoupee[0], commande_decoupee[1], commande_decoupee[2]);

    for (int i = 0; commande_decoupee[i] != NULL; i++) {
        char *args = commande_decoupee[i];

        // Supprimer les espaces inutiles autour de la commande
        while (isspace(*args)) args++;
        char *end = args + strlen(args) - 1;
        while (end > args && isspace(*end)) *end-- = '\0';

        char ** new_args = decoupe(args);

        if (strlen(new_args) > 0) {
            //printf("Exécution de la sous-commande : %s\n", args);
            printf("args = #%s#\n", args);
            result = execute_commande_quelconque(new_args, last_status, args);
            //printf("%d\n", result);
        }
    }
    return result;
}

