#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "../include/decoupeCmd.h"

#define MAX_COM 64  

char **decoupe(char *command){
    
    char **args = malloc(MAX_COM * sizeof(char*)); 
    if (args == NULL){
        perror("erreur d'allocation"); 
        return NULL;
    }
    int com_cont = 0; //nombre d'élément dans la commande 
    int inside_braces = 0;

    //découpe la commande en mot 
    char *decoupe = strtok(command, " \n"); //decoupe par espace et retour à la ligne 
    while(decoupe != NULL && com_cont < MAX_COM -1){
        /*args[com_cont++] = decoupe; 
        decoupe = strtok(NULL, " \n");//Passe à la prochaine decoupe ou retourne NULL */
        if (strcmp(decoupe, "{") == 0) {
            inside_braces = 1;
            // On commence à accumuler les parties entre accolades
            char *brace_content = malloc(strlen(decoupe) + 1);
            if (!brace_content) {
                perror("Erreur d'allocation");
                free(args);
                return NULL;
            }
            brace_content[0] = '\0'; // Initialiser la chaîne vide
            
            // Accumuler tout ce qui est entre { et }
            decoupe = strtok(NULL, " \n");
            while (decoupe != NULL && strcmp(decoupe, "}") != 0) {
                brace_content = realloc(brace_content, strlen(brace_content) + strlen(decoupe) + 2);
                strcat(brace_content, " ");
                strcat(brace_content, decoupe);
                decoupe = strtok(NULL, " \n");
            }

            // Ajouter seulement le contenu à l'intérieur des accolades
            if (brace_content[0] != '\0') {
                args[com_cont++] = brace_content;
            }
        } 
        else if (strcmp(decoupe, "}") == 0) {
            inside_braces = 0;
            args[com_cont++] = decoupe;
        } else {
            args[com_cont++] = decoupe;
        }
        decoupe = strtok(NULL, " \n");
    }
    args[com_cont] = NULL; //fin du tableau d'args avec null 

    return args;
}