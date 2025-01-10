#define _XOPEN_SOURCE 700

# include <stdlib.h> 
# include <stdio.h> 
#include <string.h>
#include <dirent.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>
#include <signal.h>

#include "../include/externe.h"
#include "../include/prompt.h"
#include "../include/pwd.h"
#include "../include/decoupeCmd.h"
#include "../include/exit.h"
#include "../include/boucle_for_simple.h"
#include "../include/cd.h"
#include "../include/ftype.h"
#include "../include/commande_structuree.h"
#include "../include/if_else.h"
#include "../include/redirection.h"
#include "../include/pipe.h"
#include "../include/signal_handlers.h"


/*void handle_signal (int signum){
    signal_recu = 1;
    last_status = 255;
}*/

int execute_commande_quelconque(char **args, int last_status){
    // Quitte la boucle si le user ecrit exit 
    if(strcmp(args[0], "exit") == 0){
        char *exit_arg; // initialisation de la val d'exit
        if (args[1] ==  NULL){
            exit_arg = NULL;
        }
        else{
            if (args[2] != NULL){
                fprintf(stderr, "exit: too many arguments\n");
                last_status = -3;
                exit_arg = NULL;
                return last_status;
            }
            else{
                exit_arg = args[1];
            }
        } 
        //printf("val de retour après exit : %d \n", func_exit(exit_arg,last_status));
        last_status = func_exit(exit_arg,last_status); // exit 
        exit(last_status);
    }
    /* Gère le cas de la commande pwwd */
    else if (strcmp(args[0], "pwd") == 0){
        // Vérifie si trop d'arguments sont fournis
        if (args[1] != NULL) {
            fprintf(stderr, "pwd: extra: invalid argument\n");
            last_status = 1;
        }
        else{
            last_status = 0;
            pwd_command();
        }
    }

    /* Gère le cas de la commande for */ 
    else if (strcmp(args[0], "for") == 0){
        last_status = boucle_for_simple(args, last_status);
    }

    else if (strcmp(args[0], "cd") == 0){
        last_status = cd(args);
    }
    else if (strcmp(args[0], "ftype") == 0){
        args[2] = NULL;
        last_status = ftype(args);
    }
    else if(strcmp(args[0], "if") == 0){
        /*if (test(args[1])) last_status = commande_externe(args[2]);
        else if (args[4] != NULL) last_status = commande_externe(args[4]);
        */
       last_status = executer_commande_if_else(args, last_status);
    }
    else{
        //printf("%s : commande invalide ou pas encore implémentée !\n", command);
        last_status = commande_externe(args);
    }

    return last_status;
}

int main(int argc, char *argv[]){
    char *command; 
    rl_outstream = stderr;
    
    struct sigaction sa;
    sigset_t mask;

    //initialisation de la structure sigaction
    memset(&sa, 0, sizeof(struct sigaction));  //réinitialisation de la structure

    //on bloque SIGTERM
    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM); //on ajoute SIGTERM au masque

    //on masque SIGTERM, tout en traitant les autres signaux
    sigprocmask(SIG_BLOCK, &mask, NULL);

   /* //on configure les gestionnaires pour tous les signaux sauf SIGTERM
    for (int sig = 1; sig < _NSIG; sig++) {
        if (sig != SIGKILL && sig != SIGSTOP) {
            if (sigaction(sig, &sa, NULL) == -1) {
                //perror("Erreur sigaction");
            }
        }
    }*/

    int last_status = 0;

    while(1){
        /* Création du prompt */ 
        char prompt[PATH_MAX + 50];
        create_prompt(last_status, prompt, sizeof(prompt));
        fprintf(stderr, "%s", prompt);
        fflush(stderr);

        

        // Lit la commande du user 
        command = readline("$ "); 
       

        if(command == NULL){
            //printf("on sort de la boucle while");
            break;
        } 

        /* Libere la memoire et passe a la prochaine iteration de la boucle */
        if (strlen(command) == 0) {
            free(command); 
            continue; 
        }

        /* Ajouter la commande à l'historique */
        if (strlen(command) > 0) {
            add_history(command);  
        }

        /* Execution d'une commande avec pipes*/
        if (is_Pipe_Command(command)){
            if (decoupe_pipe_commande(command)){
                //printf("youpi");
                last_status = execute_pipe(command, last_status);
                free(command);
            }
            else {
                //printf("bouh");
                last_status = 1;
                free(command);
            }
        }
        
        /* Execution d'une redirection */  
        else if (is_redirection(command) == 0){
            //printf("on a bien une redirection\n");
            last_status = make_redirection(command,last_status);
            free(command);
        }
        
        /* Execution d'une commande structure */
        else if (is_structured(command)){
            //printf("c'est structuré ! \n");
            int *tmp = execute_structured_command(command, last_status);
            last_status = tmp[1];
            free(tmp);
            free(command);
        }

        else{
            /* Decoupe la commande */
            char **args = decoupe(command);

            /*printf("Affichage dans fsh: \n");
            
            for (int i = 0; args[i] != NULL; i++) {
                printf("%s#", args[i]);
            }
            //printf("args[0] = %s et args[1] = %s\n", args[0], args[1]);
            printf("\n");*/

            /* Quitte la boucle si le user ecrit exit */ 
            if(strcmp(args[0], "exit") == 0){
                last_status = execute_commande_quelconque(args, last_status);
                if (last_status != -3){
                    free(command);
                    free(args);
                    return last_status;
                }
                last_status = 1;
            }
            /* Execute la commande */
            else{
                last_status = execute_commande_quelconque(args, last_status);
            } 

            /* clean la memeoire */ 
            /*for (int i = 0; args[i] != NULL; i++) {
                free(args[i]);
            }*/
            free(command);
            free(args);
        }
       
       

    }

    //fprintf(stderr, "%i", last_status);
    
    return last_status;
}
