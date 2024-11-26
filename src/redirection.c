#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/fcntl.h>

#include "../include/decoupeCmd.h"
#include "../include/fsh.h"



int is_redirection(const char *cmd){
    if(cmd == NULL || strlen(cmd) < 4){
        return 1;
    }
    //recuperation des premieres iteration des redirection
    const char *r_in = strchr(cmd, '<');
    //fprintf(stdout, " le r_in est %s \n",r_in);
    const char *r_out = strchr(cmd, '>');
    //fprintf(stdout, " le r_out est %s \n",r_out);

    //check s'il y a au moin une redirection 
    if (r_in == NULL && r_out == NULL){
    return 1;
    }

    //check s'il n'y a pas < et > en meme temps 
    if (r_in != NULL && r_out != NULL){
    return 1;
    }

    //check s'il y a bien une espace de chaque cote de < 
    if (r_in != NULL){
        if(*(r_in - 1) != ' ' && *(r_in + 1) != ' ')
        {
            return 1;
        }
    }

    // check les cas pour > 
    if ( r_out != NULL){
        // check juste avant > 
        if (r_out == cmd) {
            // check si > est en début de commande
            return 1;
        } else if (*(r_out - 1) != ' ') {
            // check si avant > n'est pas un espace
            if (!(*(r_out - 1) == '2' && (r_out - 2 >= cmd) && *(r_out - 2) == ' ')) {
                return 1; //erreur si ce n'est pas 2> avrcc un espace avant le 2
            }
        }


        // check juste apres >
        if (*(r_out + 1) != ' ' && 
        !((*(r_out + 1) == '>') && (*(r_out + 2) == ' ')) && 
        !((*(r_out + 1) == '|' ) && (*(r_out + 2) == ' '))){
        return 1;
        }
 
    }

    return 0;
}

int make_redirection(char* cmd, int last_status){
    int i;
    char **dec = decoupe(cmd);
    int fd;
    int result = 0;

    //cherche la position de la redirection dans le tableau
    for(i = 0; dec[i] != NULL ; i++){
        if ((strchr(dec[i], '>')) || (strchr(dec[i], '<'))){
            break;
        }
    }

    //printf("la case i du tab est : %s\n", dec[i]);
    //printf("le fichier text est : %s\n", dec[i+1]);


    // redirection de l'entrée standard sur un fichier
    if(strcmp(dec[i], "<") == 0){

        //sauvegarde l'entrée standar 
        int stdin_backup = dup(STDIN_FILENO);
        if (stdin_backup < 0){
            perror("erreur lors dup pour le back up"); 
            return 1;
        }

        //printf("bien rentré dans la boucle de < \n");

        //on ouvre le fichier present dans la case i+1
        fd = open(dec[i+1], O_RDONLY); 
        if (fd < 0){
            perror("erreur open"); 
            return 1;
        }

        //
        if(dup2(fd, STDIN_FILENO) < 0){
            perror("erreur dup2");
            close(fd); 
            return 1;
        }

        close(fd);
        dec[i] = NULL;
        dec[i+1] = NULL;
        
        //printf("la case i du tab est : %s\n", dec[i]);
        //printf("le fichier text est : %s\n", dec[i+1]);

        result = execute_commande_quelconque(dec,last_status,dec[0]);
        
        //printf("la val de retour de exec coms qq est : %d", result); 


        // Restauration de l'entree standar
        if(dup2(stdin_backup,STDIN_FILENO) < 0){
            perror("erreur lors de la restaurtion de stdin");
            close(stdin_backup);
            return 1;
        }
        close(stdin_backup);
    }


    // clean de toutes les allocution de memoire
    for (int i = 0; dec[i] != NULL; i++) {
        free(dec[i]);
    }
    free(dec);

    return result;
}
