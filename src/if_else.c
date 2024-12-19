#include <stdlib.h> 
#include <string.h>
#include <stdio.h>

#include "../include/fsh.h"
#include "../include/decoupeCmd.h"
#include "../include/externe.h"
#include "../include/commande_structuree.h"
#include "../include/boucle_for_simple.h"

#define MAX_COM 1024

int executer_commande_if_else (char ** args, int last_status){
    int current = 1;
    int brace = 0;

    char **test = malloc(MAX_COM * sizeof(char));
    int i;
    for (i = 1; strcmp(args[i], "{") != 0; i++){
        test[i-1] = args[i];
        current++;
    }
    test[i-1] = NULL;
    //fprintf(stderr, "Affichage de test : \n");
            
    /*for (int i = 0; test[i] != NULL; i++) {
        fprintf(stderr, "%s#", test[i]);
    }*/
    //fprintf(stderr, "\n%d\n", current);

    int result = 0;
    int execute_test = commande_externe(test);//, last_status);
    
    free(test);

    if (strcmp(args[current], "{") != 0){
        perror("il manque l'accolade entrante du if");
        return -1;
    }

    current++; // on saute l'accolade
    char **commande_if = malloc(MAX_COM * sizeof(char));
    int tmp = current;
    int size = 0;
    for (int i = tmp; args[i] != NULL; i++){
        if (strcmp(args[i], "{") == 0) brace++;
        if (strcmp(args[i], "}") == 0){
            if (brace == 0) break;
            brace--;
        }
        commande_if[i-tmp] = args[i];
        current++;
        size++;
    }
    commande_if[size] = NULL;
    if (execute_test == 0){
        //args[current];

        /*printf("Affichage de commande_if : \n");
            
        for (int i = 0; commande_if[i] != NULL; i++) {
            printf("%s#", commande_if[i]);
        }
        printf("\n");*/
        char commande_if2 [MAX_COM];
        concatenate_args(commande_if, commande_if2);
        //fprintf(stderr, "commande : %s\n", commande_for);
        //fprintf(stderr, "structurée ? %i\n", is_structured(commande_for));
        if (is_structured(commande_if2)){
            int *tmp = execute_structured_command(commande_if2, last_status);
            result = tmp[1];
            free(tmp);
        }
        else{
            result = execute_commande_quelconque(commande_if, last_status);
        }

        if (strcmp(args[current], "}") != 0){
            perror("il manque l'accolade sortante du if");
            free(commande_if);
            return -1;
        }

        current++; // on saute l'accolade
        free(commande_if);
        return result;
    }

    /*for (int i = 0; commande_if[i] != NULL; i++) {
        free(commande_if[i]);
    }*/
    free(commande_if);

    //fprintf(stderr, "current : %i\n", current);
    while (strcmp(args[current], "}") != 0){
        current++;
    }

    current++;

    if (execute_test != 0 && args[current] != NULL){
        //fprintf(stderr, "current : %i\n", current);
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
        brace = 0;
        int size = 0;
        for (int i = tmp; args[i] != NULL; i++){
            if (strcmp(args[i], "{") == 0) brace++;
            if (strcmp(args[i], "}") == 0){
                if (brace == 0) break;
                brace--;
            }
            commande_else[i-tmp] = args[i];
            current++;
            size++;
        }

        commande_else[size] = NULL;
        /*printf("Affichage de commande_else : \n");
            
        for (int i = 0; commande_else[i] != NULL; i++) {
            printf("%s#", commande_else[i]);
        }
        printf("\n%d\n", current);*/

        char commande_else2 [MAX_COM];
        concatenate_args(commande_else, commande_else2);
        //fprintf(stderr, "commande : %s\n", commande_else2);
        //fprintf(stderr, "structurée ? %i\n", is_structured(commande_for));
        if (is_structured(commande_else2)){
            int *tmp = execute_structured_command(commande_else2, last_status);
            result = tmp[1];
        }
        else{
            result = execute_commande_quelconque(commande_else, last_status);
        }

        /*for (int i = 0; commande_else[i] != NULL; i++) {
            free(commande_else[i]);
        }*/
        free(commande_else);
    }
    /*else{
        result = 0;
    }*/
    //printf("%i\n", result);
    return result;
}