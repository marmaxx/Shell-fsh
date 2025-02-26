#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "../include/src/ftype.h"

int ftype (char ** args){
    //Vérification du nom de la commande
    if (strcmp(args[0], "ftype")==0){
        //Vérification du nombre d'arguments 
        if (args[2]!=NULL){
            fprintf(stderr, "ftype : trop d'arguments");
            return -1;
        }

        //Récupération du type de la référence
        struct stat buf;
        if (lstat(args[1], &buf)!=0){
            fprintf(stderr, "la");
            //perror("ftype : erreur lors de l'exécution de lstat");
            return 1;
        }
        if (S_ISREG(buf.st_mode)) {
            printf("regular file\n");
            fflush(stdout);
        } 
        else if (S_ISDIR(buf.st_mode)) {
            printf("directory\n");
            fflush(stdout);
        } 
        else if (S_ISLNK(buf.st_mode)) {
            printf("symbolic link\n");
            fflush(stdout);
        }  
        else if (S_ISFIFO(buf.st_mode)) {
            printf("named pipe\n");
            fflush(stdout);
        }
        else {
            printf("other\n");
            fflush(stdout);
        }
        return 0;
    }
    return 1;
}