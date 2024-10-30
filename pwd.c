#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include "pwd.h"
//#include <sys/syslimits.h>

struct dirent *entry;

// Fonction récupérant le nom du répertoire courant
char *nom_du_repertoire() {
    int fd_cur = open(".", O_RDONLY);
    if (fd_cur == -1) {
        perror("Erreur lors de l'ouverture du répertoire courant");
        exit(EXIT_FAILURE);
    }

    int fd_parent = open("..", O_RDONLY);
    if (fd_parent == -1) {
        perror("Erreur lors de l'ouverture du répertoire parent");
        close(fd_cur);
        exit(EXIT_FAILURE);
    }

    struct stat stat_cur, stat_entry;
    if (fstat(fd_cur, &stat_cur) == -1) {
        perror("Erreur lors de fstat sur le répertoire courant");
        close(fd_cur);
        close(fd_parent);
        exit(EXIT_FAILURE);
    }

    DIR *dir = fdopendir(fd_parent);
    if (dir == NULL) {
        perror("Erreur lors de fdopendir sur le répertoire parent");
        close(fd_cur);
        close(fd_parent);
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    char *dir_name = NULL;

    while ((entry = readdir(dir)) != NULL) {
        if (fstatat(fd_parent, entry->d_name, &stat_entry, 0) == -1) {
            continue;
        }

        // On récupère le nom du répertoire où les numéros d'inoeuds et de volume correspondent 
        if (stat_cur.st_ino == stat_entry.st_ino && stat_cur.st_dev == stat_entry.st_dev) { 
            dir_name = strdup(entry->d_name);
            break;
        }
    }

    closedir(dir);
    close(fd_cur);
    close(fd_parent);

    if (dir_name == NULL) {
        fprintf(stderr, "Erreur: nom du répertoire courant non trouvé\n");
        exit(EXIT_FAILURE);
    }

    return dir_name;
}

// Fonction qui récupère le chemin absolu du répertoire courant
char *chemin_absolu() {
    // Sauvegarder le répertoire courant
    char saved_cwd[PATH_MAX];
    if (getcwd(saved_cwd, sizeof(saved_cwd)) == NULL) {
        perror("Erreur lors de la sauvegarde du répertoire courant");
        exit(EXIT_FAILURE);
    }

    // Vérifie si le répertoire courant est la racine 
    struct stat stat_cur, stat_parent;
    if (stat(".", &stat_cur) == -1 || stat("..", &stat_parent) == -1) {
        perror("Erreur lors de stat");
        exit(EXIT_FAILURE);
    }

    if (stat_cur.st_ino == stat_parent.st_ino && stat_cur.st_dev == stat_parent.st_dev) {
        return strdup("/"); // cas de la racine 
    }

    char *nom = nom_du_repertoire();

    // On remonte d'un niveau dans l'arborescence 
    if (chdir("..") == -1) {
        perror("Erreur lors de chdir");
        exit(EXIT_FAILURE);
    }

    char *chemin_parent = chemin_absolu();

    // On concatène le nom du répertoire courant au chemin du parent 
    size_t len = strlen(chemin_parent) + strlen(nom) + 2;
    char *chemin_complet = malloc(len);
    if (chemin_complet == NULL) {
        perror("Erreur d'allocation mémoire");
        free(chemin_complet);
        exit(EXIT_FAILURE);
    }

    // Construction du chemin complet
    sprintf(chemin_complet, "%s%s/", chemin_parent, nom);

    if (chdir(saved_cwd) == -1) {
        perror("Erreur lors de la restauration du répertoire");
        free(chemin_complet);
        exit(EXIT_FAILURE);
    }
    
    return chemin_complet;
}
