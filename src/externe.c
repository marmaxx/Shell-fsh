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
#include "../include/signal_handlers.h"


extern volatile int signal_recu;

#define MAX_COM 64 

void handle_signal_commandes (int signum){
    signal_recu = 1;
    exit(255);
}


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
        
        struct sigaction sa;
            
        //initialisation de la structure sigaction
        memset(&sa, 0, sizeof(struct sigaction)); 
        sa.sa_handler = handle_signal_commandes;  //définition du gestionnaire de signal
        
        //on configure les gestionnaires pour tous les signaux
        for (int sig = 1; sig < _NSIG; sig++) {
            if (sig != SIGKILL && sig != SIGSTOP) {
                if (sigaction(sig, &sa, NULL) == -1) {
                    //perror("Erreur sigaction");
                    //exit(1);
                }
            }
        }
        
        if(execvp(args[0], args) < 0){  //execution de la commande
            //printf("#%s#\n", args[0]);
            //printf("Code d'erreur : %d\n", errno);
            perror("redirect_exec"); 
            exit(1);
        }
        
    }

    else {

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
