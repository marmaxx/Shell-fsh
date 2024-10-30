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

int main(){
    char *command; 
    int last_status = 0;

    while(1){
        // Création du prompt
        char prompt[PATH_MAX + 50];
        create_prompt(last_status, prompt, sizeof(prompt));
        printf("%s", prompt);

        // Lit la commande du user 
        command = readline(""); 

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

        // Gère le cas de la commande pwwd
        else if (strcmp(command, "pwd") == 0){
            last_status = 0;
            free(command);
            printf("%s\n", chemin_absolu());
        }

        // Gère les autres cas 
        else if (commande_externe(command) == 0){
            last_status = 0;
            free(command);
        }
        else{
            printf("%s : commande invalide ou pas encore implémentée !\n", command);
            last_status = -1;
            free(command);
        }
    }
    
    return 0;
}