#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>

#include "../include/externe.h"
#include "../include/prompt.h"
#include "../include/decoupeCmd.h"


#define MAX_COM 64 


int commande_externe(char *command){
    char **args = decoupe(command) ; //tableau pour stocker les éléments de la commande 
    if(args == NULL){
        return -1;
    }

    if (strcmp(args[0], "pwd")!=0 ){
        //creation d'un processus enfant pour executer la commande 
        pid_t pid = fork();
        if(pid < 0 ){
            perror("Erreur de fork");
            free(args);
            return -1; 
        } else if(pid == 0 ){
            if(execvp(args[0], args) < 0){  //execution de la commande
                perror("Erreur d'execution de la commande"); 
                free(args);
                return -1;
            }
        } else {
            int status; 
            waitpid(pid, &status, 0); //processus parent attend la fin de l'execution du processus enfant 
            return 0;
        }  
    }
    free(args);
    return 0;
}
