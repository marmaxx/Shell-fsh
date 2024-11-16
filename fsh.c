# include <stdlib.h> 
# include <stdio.h> 
#include <string.h>
#include <dirent.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>

#include "../include/externe.h"
#include "../include/prompt.h"
#include "../include/pwd.h"
#include "../include/decoupeCmd.h"
#include "../include/exit.h"
#include "../include/boucle_for_simple.h"
#include "../include/cd.h"

#include "../include/ftype.h"

int main(){
    char *command; 
    int last_status = 0;
    rl_outstream = stderr;

    while(1){
        // Création du prompt
        char prompt[PATH_MAX + 50];
        create_prompt(last_status, prompt, sizeof(prompt));
        fprintf(stderr, "%s", prompt);
        fflush(stderr);

        // Lit la commande du user 
        command = readline(""); 

        if(command == NULL){
            break;
        } 

        if (strlen(command) == 0) {
            free(command);  // Libère la mémoire allouée pour une commande vide
            continue;  // Passe à la prochaine itération de la boucle sans découper une commande vide
        }

        if (strlen(command) > 0) {
            add_history(command);  // Ajouter la commande à l'historique
        }

        // decoupe la commande
        char **args = decoupe(command);

        /*printf("Affichage dans fsh: \n");
        int taille = sizeof(args) / sizeof(args[0]);
        for (int i = 0; args[i] != NULL; i++) {
            printf("%s#", args[i]);
        }
        printf("args[0] = %s et args[1] = %s\n", args[0], args[1]);
        printf("\n");*/

        // Quitte la boucle si le user ecrit exit 
        if(strcmp(args[0], "exit") == 0){
            char *exit_arg; // initialisation de la val d'exit
            if(args[1] != NULL) {
                exit_arg = args[1];
            } else {
                exit_arg = NULL;
            }
            //printf("val de retour après exit : %d \n", func_exit(exit_arg,last_status));
            return func_exit(exit_arg,last_status); // exit 
        }

        // Gère le cas de la commande pwwd
        else if (strcmp(args[0], "pwd") == 0){
            last_status = 0;
            printf("%s\n", chemin_absolu());
        }

        // Gère le cas de la commande for
        else if (strcmp(args[0], "for") == 0){
            boucle_for_simple(args[3], args[4]);
        }

        else if (strcmp(args[0], "cd") == 0){
            last_status = cd(args);
        }
        else if (strcmp(args[0], "ftype") == 0){
            last_status = ftype(args);
        }
        else{
            //printf("%s : commande invalide ou pas encore implémentée !\n", command);
            last_status = commande_externe(args);
        }

        free(command); 
        free(args); 
    }
    
    return last_status;
}
