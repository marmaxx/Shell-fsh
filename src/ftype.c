#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>


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
            perror("ftype : erreur lors de l'exécution de lstat");
        }
        if (S_ISREG(buf.st_mode)) {
        printf("regular file\n");
        } 
        else if (S_ISDIR(buf.st_mode)) {
            printf("directory\n");
        } 
        else if (S_ISLNK(buf.st_mode)) {
            printf("symbolic link\n");
        }  
        else if (S_ISFIFO(buf.st_mode)) {
            printf("named pipe\n");
        }
        else {
            printf("other\n");
        }
        return 0;
    }
    return 1;
}