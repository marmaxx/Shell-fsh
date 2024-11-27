#include <stdlib.h> 

#include "../include/fsh.h"
#include "../include/decoupeCmd.h"

int executer_commande_if_else (char ** args, int last_status){
    char *test = decoupe(args[1]);
    int result;
    int execute_test = execute_commande_quelconque(test, last_status, args[1], 1);

    if (execute_test == 0){
        char * commande_if = decoupe(args[2]);
        result = execute_commande_quelconque(commande_if, last_status, args[2], 1);
    }
    else if (execute_test != 0 && args[4] != NULL){
        char * commande_else = decoupe(args[4]);
        result = execute_commande_quelconque(commande_else, last_status, args[4], 1);
    }
    else{
        result = 0;
    }
    return result;
}