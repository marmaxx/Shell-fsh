#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "../include/src/decoupeCmd.h"

#define MAX_COM 64  

char **decoupe(char *command){
    
    char **args = malloc(MAX_COM * sizeof(char*)); 
    if (args == NULL){
        perror("erreur d'allocation"); 
        return NULL;
    }
    int com_cont = 0; //nombre d'élément dans la commande 

    //découpe la commande en mot 
    char *decoupe = strtok(command, " \n"); //decoupe par espace et retour à la ligne 
    while(decoupe != NULL && com_cont < MAX_COM -1){
        args[com_cont] = decoupe; 
        decoupe = strtok(NULL, " \n");//Passe à la prochaine decoupe ou retourne NULL 
        com_cont++;
    }
    args[com_cont] = NULL; //fin du tableau d'args avec null 

    free(decoupe);
    return args;
}
