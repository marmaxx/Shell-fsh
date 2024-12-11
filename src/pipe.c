#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#include "../include/externe.h"
#include "../include/decoupeCmd.h"
#include "../include/fsh.h"
#include "../include/pipe.h"
#include "../include/redirection.h"

char **args = NULL;
int args_count = 0;

int is_Pipe_Command(char *command) {
    // Vérification de la présence du caractère '|', pas de '||' ni '|' au début ou à la fin
    if (strchr(command, '|') == NULL) {
        //write(STDERR_FILENO, "Erreur de syntaxe : aucun '|' trouvé\n", 37);
        return 0;
    }

    if (command[0] == '|' || command[strlen(command) - 1] == '|') {
        write(STDERR_FILENO, "Erreur de syntaxe : pipe au début ou à la fin\n", 46);
        return 0;
    }

    if (strstr(command, "||") != NULL) {
        write(STDERR_FILENO, "Erreur de syntaxe : '||' trouvé\n", 33);
        return 0;
    }

    return 1;
}

int verify_spaces_around_pipes(char *command) {
    // Vérification qu'il y a un espace avant et après chaque '|' (sauf en début et fin)
    for (int i = 1; i < strlen(command) - 1; i++) {
        if (command[i] == '|') {
            if (command[i - 1] != ' ' || command[i + 1] != ' ') {
                write(STDERR_FILENO, "Erreur de syntaxe : pas d'espace avant ou après '|' \n", 54);
                return 0;
            }
        }
    }
    return 1;
}

int decoupe_pipe_commande(char *command) {
    if (!is_Pipe_Command(command)) {
        return 0;  // Commande invalide si ce n'est pas une commande avec des pipes
    }

    // Vérifier qu'il y a un espace avant et après chaque pipe
    if (!verify_spaces_around_pipes(command)) {
        return 0;  // Si la syntaxe est incorrecte, on arrête
    }

    char *token = strtok(command, "|");

    while (token != NULL) {
        // Suppression des espaces en début et fin de chaque sous-commande
        while (isspace((unsigned char)*token)) token++;
        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end)) {
            *end-- = '\0';
        }

        // Vérification si la sous-commande est vide après nettoyage
        if (strlen(token) == 0) {
            write(STDERR_FILENO, "Erreur de syntaxe : sous-commande vide\n", 39);
            free(args);
            return 0;
        }

        // Ajout de la sous-commande à args
        char **new_args = realloc(args, (args_count + 1) * sizeof(char *));
        if (new_args == NULL) {
            perror("realloc : erreur");
            free(args);
            return 0;
        }
        args = new_args;

        args[args_count] = strdup(token); // Utilisation de strdup pour éviter de modifier la chaîne source
        if (args[args_count] == NULL) {
            perror("malloc : erreur");
            for (int i = 0; i < args_count; i++) {
                free(args[i]);
            }
            free(args);
            return 0;
        }

        args_count++;
        token = strtok(NULL, "|");
    }

    // Ajouter un pointeur NULL pour marquer la fin
    char **final_args = realloc(args, (args_count + 1) * sizeof(char *));
    if (final_args == NULL) {
        perror("realloc : erreur");
        for (int i = 0; i < args_count; i++) {
            free(args[i]);
        }
        free(args);
        return 0;
    }
    args = final_args;
    args[args_count] = NULL;

    return 1;
}

int execute_pipe(char *command, int last_status) {
    int result = last_status;

    // Créer un tableau de pipes (un pipe pour chaque paire de commandes)
    int pipe_fd[args_count - 1][2]; // Nombre de pipes = nombre de commandes - 1

    for (int i = 0; i < args_count - 1; i++) {
        if (pipe(pipe_fd[i]) == -1) {
            perror("pipe");
            return 0;
        }
    }

    for (int i = 0; i < args_count; i++) {
        pid_t pid = fork();

        if (pid == 0) { // Processus enfant
            // Redirection d'entrée : si ce n'est pas le premier processus, lire depuis le pipe précédent
            if (i > 0) {
                if (dup2(pipe_fd[i - 1][0], STDIN_FILENO) == -1) {
                    perror("dup2 input");
                    exit(1);
                }
            }

            // Redirection de sortie : si ce n'est pas le dernier processus, écrire dans le pipe suivant
            if (i < args_count - 1) {
                if (dup2(pipe_fd[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2 output");
                    exit(1);
                }
            }

            // Fermeture de tous les descripteurs de pipe dans le processus enfant 
            for (int j = 0; j < args_count - 1; j++) {
                close(pipe_fd[j][0]);
                close(pipe_fd[j][1]);
            }

            // Vérification de redirection (si présente dans la commande) et exécution de la commande
            if (is_redirection(args[i])) {
                result = make_redirection(args[i], last_status);
            } else {
                char **args_i = decoupe(args[i]);
                result = execute_commande_quelconque(args_i, last_status, command);
            }

            exit(result); 
        }
    }

    // Fermeture de tous les descripteurs de pipe dans le parent
    for (int i = 0; i < args_count - 1; i++) {
        close(pipe_fd[i][0]);
        close(pipe_fd[i][1]);
    }

    // Attendre tous les processus enfants
    for (int i = 0; i < args_count; i++) {
        wait(NULL);
    }

    // Libération de la mémoire utilisée pour les arguments
    free(args);
    args = NULL;
    args_count = 0;

    return result;
}
