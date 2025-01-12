#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/fcntl.h>

#include "../include/src/decoupeCmd.h"
#include "../include/bin/fsh.h"
#include "../include/src/redirection.h"


/* Fonction qui permet de voir si une commande est une redirection simple
    ou à l'intérieur de commandes structurée . */
int is_redirection(const char *command) {
    int inside_braces = 0;

    for (int i = 0; command[i] != '\0'; i++) {
        if (command[i] == '{') {
            inside_braces++;
        } else if (command[i] == '}') {
            inside_braces--;
        } else if (command[i] == '>' || command[i] == '>') {
            if (inside_braces != 0) {
                return 1; 
            }
        }
    }

    return is_simple_redirection(command); // Une redirection à été trouvé
}


/* Fonction qui permet de vérifier que la syntaxe de la redirection soit bonne. */
int is_simple_redirection(const char *cmd){
    if(cmd == NULL || strlen(cmd) < 4){
        return 1;
    }
    /* Recuperation des premieres iteration des redirection */ 
    const char *r_in = strchr(cmd, '<');
    //fprintf(stdout, " le r_in est %s \n",r_in);
    const char *r_out = strchr(cmd, '>');
    //fprintf(stdout, " le r_out est %s \n",r_out);

    /* Check s'il y a au moin une redirection */ 
    if (r_in == NULL && r_out == NULL){
    return 1;
    }

    /* Check s'il y a bien une espace de chaque cote de < */ 
    if (r_in != NULL){
        if(*(r_in - 1) != ' ' && *(r_in + 1) != ' ')
        {
            return 1;
        }
    }

    /* Check les cas pour > */ 
    if ( r_out != NULL){
        // check juste avant > 
        if (r_out == cmd) {
            // check si > est en début de commande
            return 1;
        } else if (*(r_out - 1) != ' ') {
            // check si avant > n'est pas un espace
            if (!(*(r_out - 1) == '2' && (r_out - 2 >= cmd) && *(r_out - 2) == ' ')) {
                return 1; //erreur si ce n'est pas 2> avec un espace avant le 2
            }
        }


        /* Check juste apres > */
        if (*(r_out + 1) != ' ' && 
        !((*(r_out + 1) == '>') && (*(r_out + 2) == ' ')) && 
        !((*(r_out + 1) == '|' ) && (*(r_out + 2) == ' '))){
        return 1;
        }
 
    }

    return 0;
}


/* Fonction qui effectue la redirection . */
int make_redirection(char* cmd, int last_status){
    int nb_red = 0;
    int *tab = malloc(10 * sizeof(int)); //position de chaque signe de redirection
    char **dec = decoupe(cmd);
    int result = 0;

    /* Cherche la position des redirections dans le tableau */ 
    for(int i = 0; dec[i] != NULL ; i++){
        if ((strchr(dec[i], '>')) || (strchr(dec[i], '<'))){
            //printf(" nb_red = %i \n", nb_red);
            //printf(" i 1  = %i \n", i);
            tab[nb_red] = i;
            nb_red++; 
        }
    }

   

    //printf(" nb_red = %i \n", nb_red);
    //printf("la case i du tab est : %s\n", dec[tab[0]]);
    //printf("le fichier text est : %s\n", dec[tab[0]+1]);

    /* Sauvegarde l'entrée standar */
    int stdin_backup = dup(STDIN_FILENO);
    if (stdin_backup < 0){
        perror("erreur lors dup pour le back up"); 
        return 1;
    }

    /* Copie de la sortie standart */
    int stdout_backup = dup(STDOUT_FILENO);
    if (stdout_backup < 0) {
        perror("Erreur lors de la sauvegarde de stdout");
        return 1;
    }

    /* Copie de la sortie erreur */
    int stderr_backup = dup(STDERR_FILENO);
    if (stderr_backup < 0) {
        perror("Erreur lors de la sauvegarde de stdout");
        return 1;
    }

int n = 0; // Index de la redirection
while(nb_red > 0){
    /* Redirection de l'entrée standard sur un fichier */

    if(strcmp(dec[tab[n]], "<") == 0){
        //printf("bien rentré dans la boucle de < \n");

        /* Ouverture du fichier qu'on va rediriger en entree */
        int fd = open(dec[tab[n]+1], O_RDONLY); 
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
    }
    /* Gestion de la redirection de sortie standard */
    else if (strcmp(dec[tab[n]], ">") == 0 
    || strcmp(dec[tab[n]], ">>") == 0 
    || strcmp(dec[tab[n]], ">|") == 0) {

        /* Initialisation des flags */
        int flags = 0; 
        if (strcmp(dec[tab[n]], ">>") == 0){
            flags = (O_APPEND | O_CREAT | O_WRONLY);
        } else if (strcmp(dec[tab[n]], ">|") == 0){
            flags = (O_CREAT | O_WRONLY | O_TRUNC);
        } else {
            flags = ( O_CREAT | O_WRONLY | O_EXCL);
        }

        /* Initialisation du descripteur */
        int fd = open(dec[tab[n] + 1], flags, 0644);
        if (fd < 0) {
            perror("Erreur lors de l'ouverture du fichier pour la redirection de sortie");
            close(stdout_backup);
            return 1;
        }

        /* Rediraction de la sortie standart sur le fichier */
        if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("Erreur lors de dup2 pour redirection de sortie");
            close(fd);
            close(stdout_backup);
            return 1;
        }
        close(fd);

    } else if (strcmp(dec[tab[n]], "2>") == 0 
    || strcmp(dec[tab[n]], "2>|") == 0 
    || strcmp(dec[tab[n]], "2>>") == 0){

        /* Initialisation des flags */
        int flags; 
        if (strcmp(dec[tab[n]],"2>>") == 0){
            flags = (O_RDWR | O_CREAT | O_APPEND) ;
        } else if (strcmp(dec[tab[n]], "2>|") == 0){
            flags = (O_RDWR | O_TRUNC | O_CREAT);
        } else {
            flags = (O_RDWR | O_CREAT | O_EXCL);
        }

        /* Initialisation du descripteur */
        int fd = open(dec[tab[n]+1],flags,0644);
        if (fd < 0){
            perror("Erreur lors de la creation du decripteur pour 2> ...");
            close(stderr_backup); 
            return 1;
        }

        /* Redirection de la sortie erreur standard */
        if(dup2(fd,STDERR_FILENO) < 0){
            perror("Erreur lors du dup2 de la sortie erreur sur le decripteur");
            close(stderr_backup); 
            close(fd);
            return 1;
        }
        close(fd);
    }

    nb_red = nb_red - 1 ;
    n = n + 1; 
}


 /* Clean du tableau de commande */
    for(int j = tab[0]; dec[j] != NULL ; j++){
        dec[j] = NULL;
    }

    /* Execution de la commande */
    result = execute_commande_quelconque(dec, last_status);

    /* Restauration de l'entree standard */
    if(dup2(stdin_backup, STDIN_FILENO) < 0){
        perror("Erreur lors de la restauration de stdin"); 
        return 1;
    }

    /* Restauration de la sortie standard */
    if (dup2(stdout_backup, STDOUT_FILENO) < 0) {
        perror("Erreur lors de la restauration de stdout");
        close(stdout_backup);
        return 1;
    }

    /* Restauration de la sortie erreur */
    if (dup2(stderr_backup, STDERR_FILENO) < 0) {
        perror("Erreur lors de la restauration de stderr");
        close(stderr_backup);
        return 1;
    }
   

    close(stderr_backup);
    close(stdout_backup);  
    close(stdin_backup);


    /* Clean de toutes les allocution de memoire 
    for (int j = 0; dec[j] != NULL; j++) {
        if (dec[j] != NULL){
            free(dec[j]);
        }
    }*/
    free(dec);
    free(tab);
    return result;
}
