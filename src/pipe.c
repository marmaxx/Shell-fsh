#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#include "../include/externe.h"
#include "../include/decoupeCmd.h"
#include "../include/fsh.h"


char **args = NULL;

int isPipeCommand (char *command){ //méthode vérifiant qu'il y ait le caractère '|' dans la commande, mais pas "||" ou '|' en début ou fin
    if (!strchr(command, '|')) return 0;

    else if (command[0] == '|' || command[strlen(command)-1]=='|') return 0;

    else if (strstr(command, "||")) return 0;

    return 1;
}

int decoupe_pipe_commande (char *command){
    if (!isPipeCommand) return 1;

    char *token = strtok(command, '|');
    while (token != NULL){
        if (token[0]!=' ' || token [strlen(token)-1]!=' ') return 1;
        
    }
    
}
