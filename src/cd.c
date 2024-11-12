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

#include "../include/cd.h"
#include "../include/pwd.h"

int cd(char ** args) {

    // Vérification si la commande est 'cd'
    if (strcmp(args[0], "cd") == 0) {
        struct stat infos;
        char current_dir [PATH_MAX];

        // Cas où il n'y a pas d'argument après 'cd' : Aller vers HOME
        if (args[1] == NULL) {
            const char *home = getenv("HOME");
            if (home == NULL) {
                fprintf(stderr, "cd : erreur : HOME non définie\n");
                return -1;
            } else if (chdir(home) != 0) {
                perror("cd : erreur lors du changement de répertoire vers HOME");
                return -1;
            }
            return 0;
        }

        // Vérifie si trop d'arguments sont fournis
        if (args[2] != NULL) {
            fprintf(stderr, "cd : trop d'arguments\n");
            return -1;
        }

        // Cas où l'argument est '-' : aller au répertoire précédent et afficher le chemin absolu
        if (strcmp(args[1], "-") == 0) {
            const char *env_oldpwd = getenv("OLDPWD");
            if (env_oldpwd==NULL){
                fprintf(stderr, "cd : erreur : OLDPWD non définie \n");
                return -1;
            }
            if ((chdir(env_oldpwd)) != 0) {
                perror("cd : erreur lors du changement vers le répertoire parent");
                return -1;
            }
            printf("%s\n", env_oldpwd);  // Affiche le chemin absolu
            return 0;
        }

        // Sinon, vérifie si args[1] est un répertoire et change de répertoire
        if (stat(args[1], &infos) == 0) {
            if (S_ISDIR(infos.st_mode)) {

                // Sauvegarde l'ancien répertoire dans OLDPWD
                if (getcwd(current_dir, sizeof(current_dir)) != NULL) {
                    setenv("OLDPWD", current_dir, 1); // Met à jour OLDPWD
                }

                if (chdir(args[1]) != 0) {
                    perror("cd : erreur lors du changement de répertoire");
                    return -1;
                }
                return 0;
            } else {
                fprintf(stderr, "cd : erreur : %s n'est pas un dossier\n", args[1]);
                return -1;
            }
        } else {
            perror("cd : erreur lors de la vérification du répertoire");
            return -1;
        }
    }
    return -1;
}
