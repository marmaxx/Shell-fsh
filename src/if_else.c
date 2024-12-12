#include <stdlib.h> 
#include <string.h>
#include <stdio.h>

#include "../include/fsh.h"
#include "../include/decoupeCmd.h"

#define MAX_COM 64  

int executer_commande_if_else (char ** args, int last_status){
    int current = 1;
    int index_brace = 0;
    int size = 0;
    char **test = malloc(MAX_COM * sizeof(char));
    for (int i = 1; strcmp(args[i], "{") != 0; i++){
        test[i-1] = args[i];
        current++;
        size++;
    }
    /*printf("Affichage de test : \n");
            
    for (int i = 0; i < size; i++) {
        printf("%s#", test[i]);
    }
    printf("\n%d\n", current);*/

    int result = 0;

    int execute_test = execute_commande_quelconque(test, last_status, args[1]);

    if (strcmp(args[current], "{") != 0){
        perror("il manque l'accolade entrante du if");
        return -1;
    }

    current++; // on saute l'accolade

    if (execute_test == 0){
        char **commande_if = malloc(MAX_COM * sizeof(char));
        int tmp = current;
        size = 0;
        int bool = 1;
        for (int i = tmp; bool != 0; i++){
            if (args[i] == NULL){
                perror("il manque l'accolade sortante du if");
                return -1;
            }
            if (strcmp(args[i], "{") == 0){
                index_brace++;
                printf("indice accolade : %i , arg apres : %s\n", index_brace, args[i+1]);
            }
            if (strcmp(args[i], "}") == 0 && index_brace != 0){
                index_brace--;
                printf("indice accolade : %i , arg avant : %s\n", index_brace, args[i-1]);
            }
            commande_if[i-tmp] = args[i];
            //printf("%s\n", args[i]);
            current++;
            size++;
            if (strcmp(args[i+1], "}") == 0 && index_brace == 0) bool = 0;
        }
        //commande_if[size-1] = NULL;
        /*printf("Affichage de commande_if : \n");
            
        for (int i = 0; i < size; i++) {
            printf("%s#", commande_if[i]);
        }
        printf("\n%d\n", current);*/

        result = execute_commande_quelconque(commande_if, last_status, commande_if[0]);
        /*if (strcmp(args[current-1], "}") != 0){
            perror("il manque l'accolade sortante du if");
            return -1;
        }*/
        current++; // on saute l'accolade

        return result;
    }

    while (strcmp(args[current], "}") != 0){
        current++;
    }

    current++;

    if (execute_test != 0 && args[current] != NULL){
        //printf("args : %s\n", args[current]);
        if (strcmp(args[current], "else") != 0){
            perror("il manque le else");
            return -1;
        }

        current++; // on saute le else

        if (strcmp(args[current], "{") != 0){
            perror("il manque l'accolade entrante du else");
            return -1;
        }

        current++; // on saute le l'accolade

        char **commande_else = malloc(MAX_COM * sizeof(char));
        int tmp = current;
        index_brace = 0;
        size = 0;
        int bool = 1;
        for (int i = tmp; bool != 0; i++){
            if (args[i] == NULL){
                perror("il manque l'accolade sortante du else");
                return -1;
            }
            if (strcmp(args[i], "{") == 0){
                index_brace++;
                printf("indice accolade : %i , arg apres : %s\n", index_brace, args[i+1]);
            }
            if (strcmp(args[i], "}") == 0 && index_brace != 0){
                index_brace--;
                printf("indice accolade : %i , arg avant : %s\n", index_brace, args[i-1]);
            }
            commande_else[i-tmp] = args[i];
            current++;
            size++;
            if (strcmp(args[i+1], "}") == 0 && index_brace == 0) bool = 0;
        }
        //printf("size : %i\n", size);

        /*printf("Affichage de commande_else : \n");
            
        for (int i = 0; i < size; i++) {
            printf("%s#", commande_else[i]);
        }
        printf("\n%d\n", current);*/
        result = execute_commande_quelconque(commande_else, last_status, commande_else[0]);
    }
    else{
        result = 0;
    }
    return result;
}