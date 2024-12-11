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

int is_Pipe_Command (char *command){ //méthode vérifiant qu'il y ait le caractère '|' dans la commande, mais pas "||" ou '|' en début ou fin
    if (strchr(command, '|')==NULL) {
        write (STDERR_FILENO, "Erreur de syntaxe\n", 18);
        return 0;
    }

    else if (command[0] == '|' || command[strlen(command)-1]=='|') {
        write (STDERR_FILENO, "Erreur de syntaxe\n", 18);
        return 0;
    }

    else if (strstr(command, "||")!=NULL) {
        write (STDERR_FILENO, "Erreur de syntaxe\n", 18);
        return 0;
    }
    return 1;
}

int decoupe_pipe_commande (char *command){ //méthode vérifiant que la commande est bien valable et la découpe en sous commandes 
    if (!is_Pipe_Command(command)) return 0;

    char *token = strtok(command, "|");
    while (token != NULL){
        if (token[0]!=' ' || token[strlen(token)-1]!=' '){
            write (STDERR_FILENO, "Erreur de syntaxe\n", 18);
            return 0;
        }
        while (isspace((unsigned char)*token)) token ++;
    
        char *end = token + strlen(token) -1;
        while (end > token && isspace((unsigned char)*end)) {
            *end-- = '\0';
        }

        if (strlen(token) == 0) {
            write(STDERR_FILENO, "Erreur de syntaxe\n", 18);
            free(args);
            return 0;
        }
        
        char **new_args = realloc(args, (args_count + 1) * sizeof(char *));
        if (new_args == NULL) {
            perror("realloc : erreur");
            free(args); 
            return 0;
        }
        args = new_args;

        args[args_count] = malloc((strlen(token) + 1) * sizeof(char));
        if (args[args_count] == NULL) {
            perror("malloc : erreur");
            for (int i = 0; i < args_count; i++) {
                free(args[i]);
            }
            free(args);
            return 0;
        }
        strcpy(args[args_count], token);
        args_count++;

        token = strtok(NULL, "|");

    }

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


int execute_pipe (char *command, int last_status){
    int result = last_status;
    
    int pipe_fd[args_count][2];

    for (int i = 0; i<args_count; i++){
        pipe(pipe_fd[i]);
    }

    for (int i = 0; i<args_count; i++){

        pid_t pid = fork();

        if (pid == 0){
            
            if (i>0) dup2(pipe_fd[i-1][0], STDIN_FILENO);
            if  (i<args_count) dup2(pipe_fd[i+1][1], STDOUT_FILENO);
            
            for (int j = 0; j<args_count-1; j++){
                close (pipe_fd[j][0]);
                close (pipe_fd[j][1]);
            }
        }

        if (is_redirection(args[i])) result = make_redirection(args[i], last_status);

        else {
            char **args_i = decoupe(args[i]);
            result = execute_commande_quelconque(args_i, last_status, command);
        }
        
    }

    for (int j = 0; j<args_count-1; j++){
        close (pipe_fd[j][0]);
        close (pipe_fd[j][1]);
    }

    for (int i = 0; i<args_count-1; i++){
        wait (NULL);
    }

    free (args);
    args=NULL;
    args_count = 0;

    return result;
}
