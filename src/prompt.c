#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <signal.h>

#include "../include/pwd.h"
#include "../include/prompt.h"

#define GREEN "\001\033[32m\002"  
#define RED "\001\033[91m\002"   
#define BLUE "\001\033[34m\002"  
#define CYAN "\001\033[36m\002"   
#define RESET "\001\033[00m\002"  // Retour à la couleur normale

volatile sig_atomic_t signal_recu = 0;


// Fonction pour créer un prompt tronqué
void create_prompt(int last_status, char *prompt, size_t size) {   
    // Récupération du chemin absolu grâce à la fonction créée dans pwd.c
    char *cwd = chemin_absolu();

    // On détermine la couleur en fonction du dernier statut
    const char *status_color = (last_status == 0) ? GREEN : RED;
    const char *dir_color = (last_status == 0) ? BLUE : CYAN;
    
    char status[12];
    
    if (signal_recu){ // Si un signal est reçu, on écrit SIG comme statut
        snprintf (status, sizeof(status), "SIG");
        signal_recu = 0;
    } 
    
    // Format de retour de la commande en focntion du dernier statut
    else snprintf(status, sizeof(status), "%d", last_status);
    
    // On tronque le chemin 
    size_t max_dir_length = 25;
    char truncated_cwd[PATH_MAX];

    if (strlen(cwd) > max_dir_length) {
        snprintf(truncated_cwd, sizeof(truncated_cwd), "...%s", cwd + strlen(cwd) - max_dir_length + strlen(status) + 2);
    } else {
        snprintf(truncated_cwd, sizeof(truncated_cwd), "%s", cwd);
    }

    snprintf(prompt, size, "%s[%s]%s%s%s", status_color, status, dir_color, truncated_cwd, RESET);

    free(cwd);
}
