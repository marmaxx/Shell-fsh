#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>

#include "../include/externe.h"
#include "../include/decoupeCmd.h"
#include "../include/ftype.h"
#include "../include/fsh.h"
#include "../include/commande_structuree.h"

#define MAX_COM 1024 

char *replace_args(const char *rep, const char *arg, const char *file_name, char * pl) {
    const char *placeholder = pl;
    char *result;
    char *insert_point;
    int count = 0;
    int placeholder_len = strlen(placeholder);
    int file_name_len = strlen(file_name);
    int rep_len = (rep != NULL) ? strlen(rep) : 0;

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
        if (rep != NULL){
            // Copier rep avant file_name
            strcpy(insert_point, rep);
            insert_point += rep_len;
            // Copier le "/"
            strcpy(insert_point, "/");
            insert_point += 1;
        }
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

int has_extension(const char *filename, const char *ext) {
    size_t filename_len = strlen(filename);
    size_t ext_len = strlen(ext);

    // La longueur totale de l'extension avec le point ajouté
    size_t total_ext_len = ext_len + 1;

    // Si l'extension avec le point est plus longue que le nom de fichier, retourner 0
    if (total_ext_len > filename_len) {
        return 0;
    }

    // Comparer la fin du nom de fichier avec le point et l'extension
    return filename[filename_len - total_ext_len] == '.' &&
           strncmp(filename + (filename_len - ext_len), ext, ext_len) == 0;
}

// Fonction pour vérifier le type de fichier
int matches_type(struct dirent *entry, char *type) {
    char t = *type;
    switch (t) {
        case 'f': return entry->d_type == DT_REG; // Fichier ordinaire
        case 'd': return entry->d_type == DT_DIR; // Répertoire
        case 'l': return entry->d_type == DT_LNK; // Lien symbolique
        case 'p': return entry->d_type == DT_FIFO; // Tube
        default: return 0;
    }
}

void concatenate_args(char *args[], char *result) {
    result[0] = '\0';
    for (int i = 0; args[i] != NULL; i++) {
        strcat(result, args[i]);
        if (args[i + 1] != NULL) {
            strcat(result, " ");
        }
    }
}

int boucle_for_simple (char ** args, int last_status){
    int option_A = 0;
    int option_r = 0;
    int option_e = 0;
    int option_t = 0;
    //int option_p = 0;
    int current = 4;
    char *ext = "";
    char *type = "";
    //char *max;
    int brace = 0;
    for (int i = 0; strcmp(args[i], "{") != 0; i++){
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
                option_e = 1;
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
                option_t = 1;
                type = args[i+1];
                current += 2;
            }
            else{
                perror("il manque un argument à -t");
                return 1;
            }
        }
        /*else if (strcmp(args[i], "-p") == 0){
            if (args[i+1] != NULL){
                option_p = 1;
                max = args[i+1];
                current += 2;
            }
            else{
                perror("il manque un argument à -p");
                return 1;
            }
        }*/
    }
    
    // Afficher les options activées
    /*if (option_A) {
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
    }*/

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
    for (int i = tmp; args[i] != NULL; i++){
        if (strcmp(args[i], "{") == 0) brace++;
        if (strcmp(args[i], "}") == 0){
            if (brace == 0) break;
            brace--;
        }
        commande[i-tmp] = args[i];
        current++;
        size++; 
    }

    int result = 0;
    struct dirent * entry;
    DIR * d = opendir(rep);

    if (d == NULL){
        perror("Erreur d'ouverture du répertoire");
        return 1;
    }

    while ((entry = readdir(d)) != NULL){
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        //int index;
        // On remplace le $F par le nom du fichier courant
        char **args_with_file = malloc(MAX_COM * sizeof(char *));
        char placeholder[3];
        placeholder[0] = '$';
        placeholder[1] = args[1][0];
        placeholder[2] = '\0';
        //strcat(placeholder, "$");
        //strcat(placeholder, args[1]);
        //printf("placeholder : %s\n\n", placeholder);
        for (int i = 0; i < size; i++) {
            args_with_file[i] = replace_args(rep, commande[i], entry->d_name, placeholder);
        }
        //fprintf(stderr, "size: %i", size);
        args_with_file[size] = NULL; // Terminer le tableau par NULL
        
                
        /*fprintf(stderr, "Affichage de args_with_file : \n");
        for (int i = 0; i < size; i++) {
            fprintf(stderr, "%s#", args_with_file[i]);
        }
        fprintf(stderr, "\n");*/

         // Si -A n'est pas activé, ignorer les fichiers cachés
        if (!option_A && entry->d_name[0] == '.') {
            continue;
        }

        // Si -e est activé, vérifier l'extension
        if (option_e && !has_extension(entry->d_name, ext)) {
            continue;
        }

        // Si -r est activé, vérifier si le fichier est un répertoire
        if (option_r == 1 && entry->d_type == DT_DIR){
            //fprintf(stderr, "ancien rep: %s\n", rep);
            char * new_rep = malloc(strlen(rep) + entry->d_reclen + 1);
            new_rep[0] = '\0';
            strcat(new_rep, rep);
            strcat(new_rep, "/");
            strcat(new_rep, entry->d_name);
            //fprintf(stderr, "nouveau rep : %s\n", new_rep);
            char **args_with_rep = malloc(MAX_COM * sizeof(char *));
            int i;
            for (i = 0; args[i] !=NULL; i++) {
                args_with_rep[i] = replace_args(NULL, args[i], new_rep, rep);
            }
            free(new_rep);
            //fprintf(stderr, "size: %i", size);
            args_with_rep[i] = NULL; // Terminer le tableau par NULL
            /*fprintf(stderr, "Affichage de args_with_rep : \n");
            for (int i = 0; args[i] != NULL; i++) {
                fprintf(stderr, "%s#", args_with_rep[i]);
            }
            fprintf(stderr, "\n");*/
            last_status = boucle_for_simple(args_with_rep, last_status);
            for (int i = 0; args_with_rep[i] != NULL; i++) {
                free(args_with_rep[i]);
            }
            free(args_with_rep);
        }

        // Si -t est activé, vérifier le type de fichier
        if (option_t && !matches_type(entry, type)) {
            continue;
        }


        char commande_for [MAX_COM];
        concatenate_args(args_with_file, commande_for);
        //fprintf(stderr, "commande : %s\n", commande_for);
        if (is_structured(commande_for)){
            int *tmp = execute_structured_command(commande_for, last_status);
            result = tmp[1];
        }
        // Exécuter la commande avec les arguments modifiés
        else{ 
            result = execute_commande_quelconque(args_with_file, last_status);
        }
        // Libérer la mémoire allouée pour args_with_file
        for (int i = 0; args_with_file[i] != NULL; i++) {
            free(args_with_file[i]);
        }
        free(args_with_file);
    }

    free(commande);
    closedir(d);
    
    return result;
}