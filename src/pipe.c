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


char **args = NULL;
int args_count = 0;

int isPipeCommand (char *command){ //méthode vérifiant qu'il y ait le caractère '|' dans la commande, mais pas "||" ou '|' en début ou fin
    if (strchr(command, '|')==NULL) {
        write (stderr, "Erreur de syntaxe\n", 18);
        return 0;
    }

    else if (command[0] == '|' || command[strlen(command)-1]=='|') {
        write (stderr, "Erreur de syntaxe\n", 18);
        return 0;
    }

    else if (strstr(command, "||")!=NULL) {
        write (stderr, "Erreur de syntaxe\n", 18);
        return 0;
    }
    return 1;
}

int decoupe_pipe_commande (char *command){ //méthode vérifiant que la commande est bien valable et la découpe en sous commandes 
    if (!isPipeCommand) return 0;

    char *token = strtok(command, '|');
    while (token != NULL){
        if (token[0]!=' ' || token[strlen(token)-1]!=' '){
            write (stderr, "Erreur de syntaxe\n", 18);
            return 0;
        }
        while (isspace((unsigned char)*token)) token ++;
    
        char *end = token + strlen(token) -1;
        while (end > token && isspace((unsigned char)*end)) {
            *end-- = '\0';
        }

        if (strlen(token) == 0) {
            write(stderr, "Erreur de syntaxe\n", 18);
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
    if (!isPipeCommand || !decoupe_pipe_commande(command)) return 1;
}
