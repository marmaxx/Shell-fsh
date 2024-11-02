#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/exit.h"


void func_exit(char *arg, int last_status){
    if (arg == NULL){ // vérifie si il y a une val après le exit
        printf("Exit %d OK (val retour de la dernière commande)\n", last_status);
        exit(last_status); // exit avec la dernière val de retour
    } else {
        //convertion du char en long
        char *endptr; 
        long val = strtol(arg, &endptr, 10);
        if (endptr == arg) {
            printf("val de exit invalide\n");
        } else if (*endptr != '\0') {
            printf("val de exit invalide\n");
        } else {
            printf("Exit %d OK\n", (int)val);
            exit((int)val); // quitte avec la valeur spécifié
        }
    }
}