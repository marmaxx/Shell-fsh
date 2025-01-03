#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>


#include "../include/externe.h"
#include "../include/prompt.h"
#include "../include/decoupeCmd.h"
#include "../include/ftype.h"


#define MAX_COM 64 


int commande_externe(char **args){
    if(args == NULL){
        return 1;
    }

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
       /*sigset_t all_signals;
        //on crée un ensemble de tous les signaux
        sigfillset(&all_signals);  //sigfillset ajoute tous les signaux à l'ensemble

        //on bloque tous les signaux
        if (sigprocmask(SIG_BLOCK, &all_signals, NULL) == -1) {
            perror("Erreur: sigprocmask");
            return 1;
        }*/
       sigset_t mask;
    sigemptyset(&mask); //on crée un ensemble vide de signaux
    sigaddset(&mask, SIGTERM); //on ajoute SIGTERM à l'ensemble des signaux à masquer
    sigprocmask(SIG_BLOCK, &mask, NULL); //on bloque SIGTERM pour fsh 

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

    return 0;
}
