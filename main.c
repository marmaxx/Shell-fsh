# include <stdlib.h> 
# include <stdio.h> 
#include <string.h>
#include <dirent.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <readline/readline.h>
#include <readline/history.h>

#include "pwd.h"

int main(){
    char *command; 

    while(1){

        // Lit la commande du user 
        command = readline("$ "); 

        if(command == NULL){
            break;
        } 

        if (strlen(command) > 0) {
            add_history(command);  // Ajouter la commande à l'historique
        }

        // Ajoute la commande à l'historique
        add_history(command);


        // Quitte la boucle si le user ecrit exit 
        if(strcmp(command, "exit") == 0){
            free(command); 
            break;
        }

        // Cas de la commande pwd
        else if (strcmp(command, "pwd") == 0){
            free(command);
            printf("%s\n", chemin_absolu());
        }

        free(command);
    }
    
    return 0;
}