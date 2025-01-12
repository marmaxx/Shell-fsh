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

volatile extern pid_t pid_fils;
volatile extern int sigint_recu;
volatile extern int sigterm_recu;


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
        sa.sa_handler = SIG_DFL; // Comportement par defaut
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        if (sigaction(SIGTERM, &sa, NULL) == -1) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
        if (sigaction(SIGINT, &sa, NULL) == -1) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }


        if(execvp(args[0], args) < 0){  //execution de la commande
            //printf("#%s#\n", args[0]);
            //printf("Code d'erreur : %d\n", errno);
            perror("redirect_exec"); 
            exit(1);
        }

    }

    else {

        pid_fils = pid;
       
        int status; 
        waitpid(pid, &status, 0); //processus parent attend la fin de l'execution du processus enfant 
        
        if (WIFEXITED(status)){
            sigterm_recu = 0;
            return WEXITSTATUS(status);
        } 
        else if (WIFSIGNALED(status)) {
            //si le processus enfant a été tué par un signal, on récupère ce signal
            int sig = WTERMSIG(status);
            if (sig == SIGINT ) sigint_recu = 1;
            else if (sig == SIGTERM) sigterm_recu = 1;
            //printf("Processus enfant tué par le signal %d\n", signal_recu);
            return 255;  // Échec dans le parent
        }
        else{
            return 1; // Indique un échec dans le parent
        }
        return 0; 
    }  

    return 0;
}
