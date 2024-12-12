#include <stdlib.h> 
#include <string.h>
#include <stdio.h>

#include "../include/fsh.h"
#include "../include/decoupeCmd.h"

#define MAX_COM 64  

int executer_commande_if_else (char ** args, int last_status){
    int current = 1;

    char **test = malloc(MAX_COM * sizeof(char));
    for (int i = 1; strcmp(args[i], "{") != 0; i++){
        test[i-1] = args[i];
        current++;
    }
    /*printf("Affichage de test : \n");
            
    for (int i = 0; test[i] != NULL; i++) {
        printf("%s#", test[i]);
    }
    printf("\n%d\n", current);*/

    int result = 0;
    int execute_test = execute_commande_quelconque(test, last_status);

    if (strcmp(args[current], "{") != 0){
        perror("il manque l'accolade entrante du if");
        return -1;
    }

    current++; // on saute l'accolade

    if (execute_test == 0){
        char **commande_if = malloc(MAX_COM * sizeof(char));
        int tmp = current;
        int size = 0;
        for (int i = tmp; strcmp(args[i], "}") != 0; i++){
            commande_if[i-tmp] = args[i];
            current++;
            size++;
        }
        commande_if[size] = args[current];

        /*printf("Affichage de commande_if : \n");
            
        for (int i = 0; commande_if[i] != NULL; i++) {
            printf("%s#", commande_if[i]);
        }
        printf("\n%d\n", current);*/
        result = execute_commande_quelconque(commande_if, last_status);

        if (strcmp(args[current], "}") != 0){
            perror("il manque l'accolade sortante du if");
            return -1;
        }

        current++; // on saute l'accolade
    }

    while (strcmp(args[current], "}") != 0){
        current++;
    }

    current++;

    if (execute_test != 0 && args[current] != NULL){
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
        for (int i = tmp; strcmp(args[i], "}") != 0; i++){
            commande_else[i-tmp] = args[i];
            current++;
        }
        printf("Affichage de commande_else : \n");
            
        for (int i = 0; commande_else[i] != NULL; i++) {
            printf("%s#", commande_else[i]);
        }
        printf("\n%d\n", current);
        result = execute_commande_quelconque(commande_else, last_status);
    }
    else{
        result = 0;
    }
    return result;
}