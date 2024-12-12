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
//vérification de la présence du caractère '|', pas de '||' ni '|' au début ou à la fin
    if (strchr(command, '|') == NULL) {
        //write(STDERR_FILENO, "Erreur de syntaxe : aucun '|' trouvé\n", 37);
        return 0;
    }

    if (command[0] == '|' || command[strlen(command) - 1] == '|') {
        fprintf(stderr, "Erreur de syntaxe : pipe au début ou à la fin\n");
        return 0;
    }

    if (strstr(command, "||") != NULL) {
        fprintf(stderr, "Erreur de syntaxe : '||' trouvé\n");
        return 0;
    }

    return 1;
}

int verify_spaces_around_pipes(char *command) {
    //vérification qu'il y ait un espace avant et après chaque '|' (sauf en début et fin)
    for (int i = 1; i < strlen(command) - 1; i++) {
        if (command[i] == '|') {
            if (command[i - 1] != ' ' || command[i + 1] != ' ') {
                fprintf(stderr, "Erreur de syntaxe : pas d'espace avant ou après '|' \n");
                return 0;
            }
        }
    }
    return 1;
}

int decoupe_pipe_commande(char *command) {
    
    if (!is_Pipe_Command(command)) {
        return 0;  //commande invalide si ce n'est pas une commande avec des pipes
    }

    // Vérifier qu'il y a un espace avant et après chaque pipe
    if (!verify_spaces_around_pipes(command)) {
        return 0;  // Si la syntaxe est incorrecte, on arrête
    }

    char *token = strtok(command, "|");

    while (token != NULL) {
        //printf("Token: '%s'\n", token);  // Debug : Affiche chaque token découpé

        //suppression des espaces en début et fin de chaque sous-commande
        while (isspace((unsigned char)*token)) token++;
        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end)) {
            *end-- = '\0';
        }

        //on vérifie si la sous-commande est vide après nettoyage
        if (strlen(token) == 0) {
            fprintf(stderr, "Erreur de syntaxe : sous-commande vide\n");
            free(args);
            return 0;
        }

        //ajout de la sous-commande à args
        char **new_args = realloc(args, (args_count + 1) * sizeof(char *));
        if (new_args == NULL) {
            perror("realloc : erreur");
            free(args);
            return 0;
        }
        args = new_args;

        args[args_count] = strdup(token); //utilisation de strdup pour éviter de modifier la chaîne source
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

    //ajout d'un pointeur NULL pour marquer la fin
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

    int pipe_fd[args_count - 1][2]; //nombre de pipes = nombre de commandes - 1

    //on crée les pipes nécessaires pour chaque commande
    for (int i = 0; i < args_count - 1; i++) {
        if (pipe(pipe_fd[i]) == -1) {
            perror("pipe");
            return 1;
        }
    }

    for (int i = 0; i < args_count; i++) {
        pid_t pid = fork();

        if (pid == 0) { //processus enfant
            
            //printf("Exécution de la commande: %s\n", args[i]); 

            //redirection d'entrée : si ce n'est pas le premier processus, lire depuis le pipe précédent
            if (i > 0) {
                if (dup2(pipe_fd[i - 1][0], STDIN_FILENO) == -1) {
                    perror("dup2 input");
                    _exit(1);
                }
            }

            //redirection de sortie : si ce n'est pas le dernier processus, on écrit dans le pipe suivant
            if (i < args_count - 1) {
                if (dup2(pipe_fd[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2 output");
                    _exit(1);
                }
            }

            //on ferme tous les descripteurs de pipe dans le processus enfant
            for (int j = 0; j < args_count-1; j++) {
                close(pipe_fd[j][0]);
                close(pipe_fd[j][1]);
            }

            //exécution de la commande
            if (!is_redirection(args[i])) { //si c'est une redirection, on gère la redirection
                //fprintf(stderr, "olala");
                _exit(make_redirection(args[i], last_status));
                
            } else { //sinon, on exécute la commande "quelconque"
                //fprintf(stderr, "On execute la commande : %s\n", args[i]);
                char **args_i = decoupe(args[i]);
                _exit(execute_commande_quelconque(args_i, last_status, command));
            
                //printf("%i", result);
            } 
        }
        
        else if (pid < 0){
            perror ("fork");
            return 1;
        }
    }
    
    //fermeture des pipes dans le parent
    for (int i = 0; i < args_count-1; i++) {
        close(pipe_fd[i][0]);
        close(pipe_fd[i][1]);
    }

    int result = 0;

    for (int i = 0; i < args_count; i++) {
        int status;
        pid_t pid = wait(&status);
        if (pid > 0) {
            //on récupère la valeur de retour de l'enfant
            if (WIFEXITED(status)) {
                result = WEXITSTATUS(status); //on met à jour le résultat
                //printf("%i\n", result);
            } else {
                result = 1; //si le processus enfant a échoué
            }
        }
    }

    //libération de la mémoire utilisée pour les arguments
    free(args);
    args = NULL;
    args_count = 0;

    //printf("%i\n", result);
    return result;
}
