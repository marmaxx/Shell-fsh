#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>

#include "../include/externe.h"
#include "../include/decoupeCmd.h"
#include "../include/ftype.h"

#define MAX_COM 64 

char *replace_args(const char *arg, const char *file_name) {
    const char *placeholder = "$F";
    char *result;
    char *insert_point;
    int count = 0;
    int placeholder_len = strlen(placeholder);
    int file_name_len = strlen(file_name);

    // Compter le nombre d'occurrences de "$F" dans arg
    for (const char *tmp = arg; (tmp = strstr(tmp, placeholder)); ++tmp) {
        count++;
    }

    // Calculer la taille de la nouvelle chaîne
    result = malloc(strlen(arg) + (file_name_len - placeholder_len) * count + 1);
    if (result == NULL) {
        perror("Erreur d'allocation");
        exit(1);
    }

    // Remplacer chaque occurrence de "$F" par file_name
    insert_point = result;
    while (count--) {
        const char *p = strstr(arg, placeholder);
        int len_before_placeholder = p - arg;
        // Copier la partie avant "$F"
        strncpy(insert_point, arg, len_before_placeholder);
        insert_point += len_before_placeholder;
        // Copier file_name à la place de "$F"
        strcpy(insert_point, file_name);
        insert_point += file_name_len;
        // Avancer après "$F" dans arg
        arg = p + placeholder_len;
    }
    // Copier le reste de arg
    strcpy(insert_point, arg);
    return result;
}

char *ajouter_rep(const char *rep, const char *file_name) {
    // Calculer la longueur totale de la chaîne résultante
    size_t rep_len = strlen(rep);
    size_t file_name_len = strlen(file_name);
    
    // Allouer de la mémoire pour la nouvelle chaîne (rep/ + file_name + '\0')
    char *result = malloc(rep_len + file_name_len + 2);  // +2 pour '/' et '\0'
    if (result == NULL) {
        perror("Erreur d'allocation");
        exit(1);
    }

    // Construire la nouvelle chaîne
    strcpy(result, rep);       // Copier rep
    strcat(result, "/");       // Ajouter '/'
    strcat(result, file_name); // Ajouter le nom du fichier

    return result;
}

void boucle_for_simple (const char * rep, char * cmd){
    char **args = decoupe(cmd);

    struct dirent * entry;
    DIR * d = opendir(rep);

    if (d == NULL){
        perror("Erreur d'ouverture du répertoire");
        return;
    }

    while ((entry = readdir(d)) != NULL){
        // On ne prend pas en compte les fichiers cachés
        if (entry->d_name[0] == '.') continue;
        
        // On remplace le $F par le nom du fichier courant
        char **args_with_file = malloc(MAX_COM * sizeof(char *));
        for (int i = 0; args[i] != NULL; i++) {
            args_with_file[i] = replace_args(args[i], entry->d_name);
        }
        args_with_file[MAX_COM - 1] = NULL; // Terminer le tableau par NULL
        
        args_with_file[1] = ajouter_rep(rep, args_with_file[1]);
        
        /*printf("Affichage dans for: \n");
        int taille = sizeof(args_with_file) / sizeof(args_with_file[0]);
        for (int i = 0; args_with_file[i] != NULL; i++) {
            printf("%s#", args_with_file[i]);
        }
        printf("\n");*/

        // Exécuter la commande avec les arguments modifiés
        if (strcmp(args_with_file[0], "ftype") == 0){
            ftype(args_with_file);
        }
        else{
            commande_externe(args_with_file);
        }

        // Libérer la mémoire allouée pour args_with_file
        for (int i = 0; args_with_file[i] != NULL; i++) {
            free(args_with_file[i]);
        }
        free(args_with_file);
    }

    closedir(d);
}