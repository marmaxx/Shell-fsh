#include <stdlib.h> 
#include <string.h>
#include <stdio.h>

#include "../include/fsh.h"
#include "../include/decoupeCmd.h"
#include "../include/externe.h"
#include "../include/commande_structuree.h"
#include "../include/boucle_for.h"
#include "../include/redirection.h"
#include "../include/if_else.h"

#define MAX_COM 1024

int executer_commande_if_else (char ** args, int last_status){
    int current = 1;
    int brace = 0;

    // On récupère la commande qui correspond au test 
    char **test = malloc(MAX_COM * sizeof(char));
    int i;
    for (i = 1; strcmp(args[i], "{") != 0; i++){
        test[i-1] = args[i];
        current++;
    }
    test[i-1] = NULL;

    // On exécute ce test 
    int result = 0;
    int execute_test;
    char new_test [MAX_COM];
    concatenate_args(test, new_test);
    if (is_redirection(new_test) == 0){
        execute_test = make_redirection(new_test, last_status);
    }
    else{ 
        execute_test = commande_externe(test);
    }
    free(test);

    if (strcmp(args[current], "{") != 0){
        perror("il manque l'accolade entrante du if");
        return -1;
    }

    // On récupère la commande qui correspond à ce qu'il faut faire si le test réussit
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

    // On exécute cette commande si le test réussit
    if (execute_test == 0){
        char commande_if2 [MAX_COM];
        concatenate_args(commande_if, commande_if2);

        if (is_structured(commande_if2)){
            int *tmp = execute_structured_command(commande_if2, last_status);
            result = tmp[1];
            free(tmp);
        }
        else if (is_redirection(commande_if2) == 0){
            result = make_redirection(commande_if2, last_status);
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

    free(commande_if);

    while (strcmp(args[current], "}") != 0){
        current++;
    }

    current++;

    // Si le test ne réussit pas alors on exécute ce qu'il y a dans le else s'il existe
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

        // On récupère la commande qui correspond à ce qu'il faut faire si le test ne réussit pas 
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
        
        // On exécute la commande 
        char commande_else2 [MAX_COM];
        concatenate_args(commande_else, commande_else2);
        if (is_structured(commande_else2)){
            int *tmp = execute_structured_command(commande_else2, last_status);
            result = tmp[1];
        }
        else if (is_redirection(commande_else2) == 0){
            result = make_redirection(commande_else2, last_status);
        }
        else{
            result = execute_commande_quelconque(commande_else, last_status);
        }

        free(commande_else);
    }

    return result;
}