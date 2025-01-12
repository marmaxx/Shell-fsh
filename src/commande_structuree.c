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

volatile extern int sigint_recu;

int is_structured(const char *command) {
    int inside_braces = 1;

    for (int i = 0; command[i] != '\0'; i++) {
        if (command[i] == '{') {
            inside_braces = 0;
        } else if (command[i] == '}') {
            inside_braces = 1;
        } else if (command[i] == ';') {
            if (inside_braces) {
                return 1; // Un point-virgule trouvé à l'intérieur des accolades
            }
        }
    }

    return 0; // Aucun point-virgule trouvé à l'intérieur des accolades
}

// Fonction pour enlever les espaces autour de la chaîne
char* trim_whitespace(char *str) {
    while(isspace((unsigned char)*str)) str++;  // Enlever les espaces au début
    if(*str == 0)  // Si la chaîne est vide après avoir enlevé les espaces
        return str;
    char *end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;  // Enlever les espaces à la fin
    *(end+1) = 0;
    return str;
}

// Fonction pour vérifier si le caractère courant est dans une accolade
int is_in_braces(const char *command, int index) {
    int brace_count = 0;
    for (int i = 0; i < index; i++) {
        if (command[i] == '{') {
            brace_count++;
        } else if (command[i] == '}') {
            brace_count--;
        }
    }
    return brace_count > 0;  // Si on est à l'intérieur d'une accolade, renvoyer vrai
}

// Fonction pour découper la commande en sous-commandes tout en tenant compte des accolades
char **decoupe_commande_structuree(const char *command) {
    char **sub_commands = malloc(MAX_COM * sizeof(char *));
    if (!sub_commands) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    int cmd_count = 0;
    int start_index = 0;
    int length = strlen(command);

    for (int i = 0; i < length; i++) {
        // Si on trouve un point-virgule et qu'on n'est pas dans des accolades
        if (command[i] == ';' && !is_in_braces(command, i)) {
            // On crée une sous-commande entre start_index et i
            int sub_cmd_length = i - start_index;
            char *sub_command = malloc(sub_cmd_length + 1);
            if (!sub_command) {
                perror("Erreur d'allocation mémoire pour la sous-commande");
                exit(EXIT_FAILURE);
            }
            strncpy(sub_command, &command[start_index], sub_cmd_length);
            sub_command[sub_cmd_length] = '\0';
            sub_commands[cmd_count] = strdup(trim_whitespace(sub_command));
            cmd_count++;
            free(sub_command);

            // Mettre à jour le point de départ de la prochaine sous-commande
            start_index = i + 1;
        }
    }

    // Ajouter la dernière sous-commande, qui ne se termine pas par un point-virgule
    if (start_index < length) {
        sub_commands[cmd_count] = strdup(trim_whitespace(&command[start_index]));
        cmd_count++;
    }

    sub_commands[cmd_count] = NULL; // Terminer le tableau avec NULL
    return sub_commands;
}

int *execute_structured_command(const char *command, int last_status){
    int *result = malloc (2 * sizeof(int));
    result[0] = 0;
    char **commande_decoupee = decoupe_commande_structuree(command);

    for (int i = 0; commande_decoupee[i] != NULL; i++) {
        char *args = commande_decoupee[i];

        // Supprimer les espaces inutiles autour de la commande
        while (isspace(*args)) args++;
        char *end = args + strlen(args) - 1;
        while (end > args && isspace(*end)) *end-- = '\0';

        char ** new_args = decoupe(args);
        /*for (int i = 0; new_args[i] != NULL; i++){
            fprintf(stderr, "%s#", new_args[i]);
        }
        fprintf(stderr, "\n");*/
        //printf("Exécution de la sous-commande : %s\n", args);
        //printf("args = #%s#\n", args);
        if (strcmp(new_args[0], "exit") == 0){
            result[0] = 1;
            result[1]= execute_commande_quelconque(new_args, last_status);
            return result;
        }
        else{
            result[1] = execute_commande_quelconque(new_args, last_status);
        }
        
        free(new_args);
        
        if (sigint_recu){ 
            for (int j = 0; commande_decoupee[j] != NULL; j++){
                free(commande_decoupee[j]);
                commande_decoupee[j] = NULL;
            }
            free(commande_decoupee);
           
            return 255;
        }
    }

    for (int i = 0; commande_decoupee[i] != NULL; i++) {
        free(commande_decoupee[i]);
    }
    free(commande_decoupee);
    return result;
}