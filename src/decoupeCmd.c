#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "../include/decoupeCmd.h"

#define MAX_COM 64  

char **decoupe(char *command1) {
    char **args = malloc(MAX_COM * sizeof(char *));
    int com_cont = 0; // nombre d'éléments dans la commande
    int in_braces = 0; // Indicateur pour savoir si nous sommes dans une accolade
    char * command = strdup(command1);
    char *start = command;  // Pointeur pour marquer le début de chaque mot ou chaîne
    char *decoupe = command;

    while (*decoupe != '\0' && com_cont < MAX_COM - 1) {
        // Si nous rencontrons un espace ou un retour à la ligne, et nous ne sommes pas dans des accolades
        if ((*decoupe == ' ' || *decoupe == '\n') && !in_braces) {
            if (decoupe > start) {
                args[com_cont++] = strndup(start, decoupe - start); // Ajoute l'argument
            }
            // Ignore les espaces ou nouvelles lignes
            start = decoupe + 1;
        }
        
        // Si on rencontre une accolade ouvrante, on démarre le morceau entre accolades
        else if (*decoupe == '{') {
            in_braces = 1; // On entre dans une portion entre accolades
            start = decoupe + 1; // Ignore l'accolade ouvrante
        }

        // Si on rencontre une accolade fermante, on termine le morceau entre accolades
        else if (*decoupe == '}') {
            if (in_braces) {
                in_braces = 0; // On sort de la portion entre accolades
                args[com_cont++] = strndup(start, decoupe - start); // Ajoute le contenu entre accolades
            }
            start = decoupe + 1; // Ignore l'accolade fermante
        }
        
        decoupe++; // Passe à l'élément suivant
    }

    // Si la commande se termine sans espace, ajouter le dernier argument
    if (decoupe > start) {
        args[com_cont++] = strndup(start, decoupe - start);
    }

    args[com_cont] = NULL; // Fin du tableau d'arguments avec NULL
    free(command);
    return args;
}
