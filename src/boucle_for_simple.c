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

char *replace_args(const char *rep, const char *arg, const char *file_name) {
    const char *placeholder = "$F";
    char *result;
    char *insert_point;
    int count = 0;
    int placeholder_len = strlen(placeholder);
    int file_name_len = strlen(file_name);
    int rep_len = strlen(rep);

    // Compter le nombre d'occurrences de "$F" dans arg
    for (const char *tmp = arg; (tmp = strstr(tmp, placeholder)); ++tmp) {
        count++;
    }

    // Calculer la taille de la nouvelle chaîne
    result = malloc(strlen(arg) + (rep_len + file_name_len - placeholder_len) * count + 2);
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
        // Copier rep avant file_name
        strcpy(insert_point, rep);
        insert_point += rep_len;
        // Copier le "/"
        strcpy(insert_point, "/");
        insert_point += 1;
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

int boucle_for_simple (char ** args, int last_status, char * cmd){
    int current = 4;
    int option_A = 0;
    int option_r = 0;
    char *ext = NULL;
    char *type = NULL;
    for (int i = 0; args[i] != NULL; i++){
        if (strcmp(args[i], "-A") == 0){
            option_A = 1;
            current++;
        }
        else if (strcmp(args[i], "-r") == 0){
            option_r = 1;
            current++;
        }
        else if (strcmp(args[i], "-e") == 0){
            if (args[i+1] != NULL){
                ext = args[i+1];
                current += 2;
            }
            else{
                perror("il manque un argument à -e");
                return 1;
            }
        }
        else if (strcmp(args[i], "-t") == 0){
            if (args[i+1] != NULL){
                type = args[i+1];
                current += 2;
            }
            else{
                perror("il manque un argument à -t");
                return 1;
            }
        }
    }
    
    // Afficher les options activées
    if (option_A) {
        printf("Option -A activée.\n");
    }

    if (option_r) {
        printf("Option -r activée.\n");
    }

    if (ext != NULL) {
        printf("Option -e avec extension : %s\n", ext);
    }

    if (type != NULL) {
        printf("Option -t avec type : %s\n", type);
    }
    //char **args = decoupe(cmd);
    /*printf("Affichage dans for: \n");
            
    for (int i = 0; args[i] != NULL; i++) {
        printf("%s#", args[i]);
    }
    printf("\n");*/

    char *rep = args[3]; // on récupère le nom du répertoire 

    if (strcmp(args[current], "{") != 0){
        perror("il manque l'accolade entrante du for");
        return -1;
    }

    current++; // on saute l'accolade

    char **commande = malloc(MAX_COM * sizeof(char));
    int tmp = current;
    int size = 0;
    for (int i = tmp; strcmp(args[i], "}") != 0; i++){
        commande[i-tmp] = args[i];
        current++;
        size++;
    }

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
        //int index;
        // On remplace le $F par le nom du fichier courant
        char **args_with_file = malloc(MAX_COM * sizeof(char *));
        for (int i = 0; i < size; i++) {
            args_with_file[i] = replace_args(rep, commande[i], entry->d_name);
        }
        //fprintf(stderr, "size: %i", size);
        args_with_file[size] = NULL; // Terminer le tableau par NULL
                
        /*fprintf(stderr, "Affichage de args_with_file : \n");
        for (int i = 0; i < size; i++) {
            fprintf(stderr, "%s#", args_with_file[i]);
        }
        fprintf(stderr, "\n");*/

        // Exécuter la commande avec les arguments modifiés
        result = execute_commande_quelconque(args_with_file, last_status, cmd);

        /*if (strcmp(args_with_file[0], "ftype") == 0){
            args_with_file[2] = NULL;
            ftype(args_with_file);
        }
        else{
            commande_externe(args_with_file);
        }*/

        // Libérer la mémoire allouée pour args_with_file
        /*for (int i = 0; args_with_file[i] != NULL; i++) {
            free(args_with_file[i]);
        }
        free(args_with_file);*/
    }

    free(commande);
    closedir(d);
    
    return result;
}