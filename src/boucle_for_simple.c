#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>

#include "../include/externe.h"
#include "../include/decoupeCmd.h"
#include "../include/ftype.h"
#include "../include/fsh.h"

#define MAX_COM 128 

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
    size_t rep_len = strlen(rep);
    size_t file_name_len = strlen(file_name);
    
    char *result = malloc(rep_len + file_name_len + 2);  // +2 pour '/' et '\0'
    if (result == NULL) {
        perror("Erreur d'allocation");
        exit(1);
    }

    // On construit la nouvelle chaîne
    strcpy(result, rep);   
    strcat(result, "/");      
    strcat(result, file_name); 

    return result;
}

int find_index_file(char **array, const char *target) {
    //int taille = sizeof(array) / sizeof(array[0]);
    for (int i = 0; array[i] != NULL; i++) {
        if (strstr(array[i], target) != NULL) {
            return i; // Retourne l'indice si trouvé
        }
    }
    return -1; // Retourne -1 si non trouvé
}

int boucle_for_simple (const char * rep, char * cmd, int last_status){
    char **args = decoupe(cmd);
    int result;
    struct dirent * entry;
    DIR * d = opendir(rep);

    if (d == NULL){
        perror("Erreur d'ouverture du répertoire");
        return 1;
    }

    while ((entry = readdir(d)) != NULL){
        // On ne prend pas en compte les fichiers cachés
        if (entry->d_name[0] == '.') continue;
        int index;
        // On remplace le $F par le nom du fichier courant
        char **args_with_file = malloc(MAX_COM * sizeof(char *));
        for (int i = 0; args[i] != NULL; i++) {
            if (find_index_file(args, "$F") == i){
                index = i;
            }
            args_with_file[i] = replace_args(args[i], entry->d_name);
        }
        args_with_file[MAX_COM - 1] = NULL; // Terminer le tableau par NULL
        
        args_with_file[index] = ajouter_rep(rep, args_with_file[index]);
        
        /*printf("Affichage dans for: \n");
        int taille = sizeof(args_with_file) / sizeof(args_with_file[0]);
        for (int i = 0; args_with_file[i] != NULL; i++) {
            printf("%s#", args_with_file[i]);
        }
        printf("\n");*/

        // Exécuter la commande avec les arguments modifiés
        result = execute_commande_quelconque(args_with_file, last_status, cmd);
    
        free(args_with_file);
    }

    closedir(d);
    
    return result;
}