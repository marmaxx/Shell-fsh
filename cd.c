#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <linux/limits.h>
#define MAX_COM 64 

#include "cd.h"
#include "pwd.h"

void cd(char *command) {
    char *args[MAX_COM];  // tableau pour stocker les éléments de la commande 
    int com_cont = 0;     // nombre d'éléments dans la commande 

    // Découpe la commande en mots
    char *decoupe = strtok(command, " \n");
    while (decoupe != NULL && com_cont < MAX_COM - 1) {
        args[com_cont++] = decoupe;
        decoupe = strtok(NULL, " \n");
    }
    args[com_cont] = NULL;  // fin du tableau d'args avec null

    // Vérification si la commande est 'cd'
    if (strcmp(args[0], "cd") == 0) {
        struct stat infos;

        // Cas où il n'y a pas d'argument après 'cd' : Aller vers HOME
        if (args[1] == NULL) {
            const char *home = getenv("HOME");
            if (home == NULL) {
                fprintf(stderr, "cd : erreur : HOME non définie\n");
            } else if (chdir(home) != 0) {
                perror("cd : erreur lors du changement de répertoire vers HOME");
            }
            return;
        }

        // Vérifie si trop d'arguments sont fournis
        if (args[2] != NULL) {
            fprintf(stderr, "cd : trop d'arguments\n");
            return;
        }

        // Cas où l'argument est '-' : aller au répertoire parent et afficher le chemin absolu
        if (strcmp(args[1], "-") == 0) {
            if (chdir("..") != 0) {
                perror("cd : erreur lors du changement vers le répertoire parent");
            }
            chemin_absolu();  // Appelle la fonction pour afficher le chemin absolu
            return;
        }

        // Sinon, vérifie si args[1] est un répertoire et change de répertoire
        if (stat(args[1], &infos) == 0) {
            if (S_ISDIR(infos.st_mode)) {
                if (chdir(args[1]) != 0) {
                    perror("cd : erreur lors du changement de répertoire");
                }
            } else {
                fprintf(stderr, "cd : erreur : %s n'est pas un dossier\n", args[1]);
            }
        } else {
            perror("cd : erreur lors de la vérification du répertoire");
        }
    }
}
