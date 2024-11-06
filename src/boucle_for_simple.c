#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>

#include "../include/externe.h"
#include "../include/decoupeCmd.h"

void boucle_for_simple (const char * rep, const char * cmd){
    char **args = decoupe(cmd);

    struct dirent * entry;
    DIR * d = opendir(rep);

    if (d == NULL){
        perror("Erreur d'ouverture du répertoire");
        return;
    }

    while ((entry = readdir(d)) != NULL){
        if (entry->d_name[0] == '.') continue;

        commande_externe(args[0]);
    }

    closedir(d);
}