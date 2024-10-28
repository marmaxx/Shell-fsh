#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "externe.h"

#define MAX_COM 64 


void commande_externe(char *command){
    char *args[MAX_COM]; //tableau pour stocker les éléments de la commande 
    int com_cont = 0; //nombre d'élément dans la commande 


    //découpe la commande en mot 
    char *decoupe = strtok(command, " \n"); //decoupe par espace et retour à la ligne 
    while(decoupe != NULL && com_cont < MAX_COM -1){
        args[com_cont++] = decoupe; 
        decoupe = strtok(NULL, " \n");//Passe à la prochaine decoupe ou retourne NULL 
    }
    args[com_cont] = NULL; //fin du tableau d'args avec null 

    if (strcmp(args[0], "pwd")!=0 ){
    //creation d'un processus enfant pour executer la commande 
    pid_t pid = fork();
    if(pid < 0 ){
        perror("Erreur de fork"); 
    } else if(pid == 0 ){
        if( execvp(args[0], args) < 0){  //execution de la commande 
            perror("Erreur d'execution de la commande"); 
            exit(EXIT_FAILURE); 
        }
    } else {
        int status; 
        waitpid(pid, &status, 0); //processus parent attend la fin de l'execution du processus enfant 
    }  
    }
}
