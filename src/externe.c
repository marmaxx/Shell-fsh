#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>
#include <sys/wait.h>
#include <errno.h>


#include "../include/externe.h"
#include "../include/prompt.h"
#include "../include/decoupeCmd.h"
#include "../include/ftype.h"


#define MAX_COM 64 


int commande_externe(char **args){
    if(args == NULL){
        return 1;
    }

    //if (strcmp(args[0], "pwd")!=0 && strcmp(args[0], "exit")!=0 && strcmp(args[0], "cd")!=0 && strcmp(args[0], "ftype")!=0){
        //creation d'un processus enfant pour executer la commande 
        pid_t pid = fork();
        if(pid < 0 ){
            perror("Erreur de fork");
            exit(1);
        } else if(pid == 0 ){
            if(execvp(args[0], args) < 0){  //execution de la commande
                //printf("#%s#\n", args[0]);
                //printf("Code d'erreur : %d\n", errno);
                perror("redirect_exec"); 
                exit(1);
            }
        } else {
            int status; 
            waitpid(pid, &status, 0); //processus parent attend la fin de l'execution du processus enfant 
            
            if (WIFEXITED(status)){
                return WEXITSTATUS(status);
            } 
            else{
                return 1; // Indique un échec dans le parent
            }
            return 0; 
        }  
    //}


    return 0;
}
