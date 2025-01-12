#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>

#include "../include/externe.h"
#include "../include/decoupeCmd.h"
#include "../include/fsh.h"
#include "../include/pipe.h"
#include "../include/redirection.h"

char **args = NULL;
int args_count = 0;

int is_Pipe_Command(char *command) {
//vérification de la présence du caractère '|', pas de '||' ni '|' au début ou à la fin
    if (strstr(command, " | ") == NULL) {
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


int decoupe_pipe_commande(char *command) { //découpe la commande et renvoie un booléen qui indique si la commande est valide ou non
    
    /*if (!is_Pipe_Command(command)) {
        return 0;  //commande invalide si ce n'est pas une commande avec des pipes
    }*/

    char *end;
    char *start = command;
    //découpage de la commande à chaque " | "
    while ((end = strstr(start, " | ")) != NULL) {
        *end = '\0';  // on termine la chaîne après le " | "

        //on ajoute la sous-commande à args
        char **new_args = realloc(args, (args_count + 1) * sizeof(char *));
        if (new_args == NULL) {
            perror("realloc : erreur");
            free(args);
            return 0;
        }
        args = new_args;

        args[args_count] = strdup(start); //on sauvegarde la sous-commande
        if (args[args_count] == NULL) {
            perror("strdup : erreur");
            for (int i = 0; i < args_count; i++) {
                free(args[i]);
            }
            free(args);
            return 0;
        }
        
        args_count++;   
        
        //printf("Debug: Commande analysée: '%s'\n", start);
        if (strchr(start, '|') != NULL && is_redirection(start)){
            fprintf(stderr, "Erreur de syntaxe: pas d'espaces entre les |\n");
            for (int i = 0; i < args_count; i++) {
                free(args[i]);
            }
            free(args);
            return 0;
        } 
        
        //on avance "start" après le " | "
        start = end + 3;
    }

    //printf("Debug: Commande analysée: '%s'\n", start);
    if (strchr(start, '|') != NULL && is_redirection(start)){
        fprintf(stderr, "Erreur de syntaxe: pas d'espaces entre les |\n");
        for (int i = 0; i < args_count; i++) {
            free(args[i]);
        }
        free(args);
        return 0;
    }

    //on ajoute la dernière sous-commande
    char **new_args = realloc(args, (args_count + 1) * sizeof(char *));
    if (new_args == NULL) {
        perror("realloc : erreur");
        for (int i = 0; i < args_count; i++) {
            free(args[i]);
        }
        free(args);
        return 0;
    }
    args = new_args;

    args[args_count] = strdup(start); //on sauvegarde la dernière sous-commande
    if (args[args_count] == NULL) {
        perror("strdup : erreur");
        for (int i = 0; i < args_count; i++) {
            free(args[i]);
        }
        free(args);
        return 0;
    }
    args_count++;

    //on marque la fin de la liste d'arguments
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
    args[args_count] = NULL; //ajout d'un dernier élément NULL pour la fin des arguments

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

    for (int i = args_count - 1; i >= 0; i--) {
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
                int res = execute_commande_quelconque(args_i, last_status);
                //printf("val de retour: %i\n", res);
                _exit(res/* = execute_commande_quelconque(args_i, last_status)*/);
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

    for (int i = args_count - 1; i>=0; i--){
        int status;
        pid_t pid = wait(&status);
        if (pid > 0 && i == args_count - 1) {
            //on récupère la valeur de retour de l'enfant pour la dernière commande
            if (WIFEXITED(status)) {
                result = WEXITSTATUS(status); //on met à jour le résultat
                //printf("%i\n", result);
            } else {
                //printf("l'enfant a échoué");
                result = 1; //si le processus enfant a échoué
            }
        }
    }

    //libération de la mémoire utilisée pour les arguments
    for (int i = 0; i < args_count; i++) {
        free(args[i]);
    }
    free(args);
    args = NULL;
    args_count = 0;

    //printf("%i\n", result);
    return result; //on renvoie le résultat de la dernière commande
}
