#define _XOPEN_SOURCE 700
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

char status[12];

void handle_signal_prompt (int signum){
    snprintf(status, 4, "SIG");
}

// Fonction pour créer un prompt tronqué
void create_prompt(int last_status, char *prompt, size_t size) {   
    // Récupération du chemin absolu grâce à la fonction créée dans pwd.c
    char *cwd = chemin_absolu();

    // On détermine la couleur en fonction du dernier statut
    const char *status_color = (last_status == 0) ? GREEN : RED;
    const char *dir_color = (last_status == 0) ? BLUE : CYAN;

    // Format de retour de la commande en focntion du dernier statut
    
    snprintf(status, sizeof(status), "%d", last_status);

    struct sigaction sa;
    
    // Initialiser la structure sigaction
    memset(&sa, 0, sizeof(struct sigaction));  // Remplir la structure de 0
    sa.sa_handler = handle_signal_prompt;  // Spécifie la fonction de gestion
    sa.sa_flags = 0;  // Aucune option spéciale
    sigemptyset(&sa.sa_mask);  // Ne bloque aucun signal pendant le traitement

    // Gestion des signaux réels (SIGRTMIN à SIGRTMAX)
    for (int sig = SIGRTMIN; sig <= SIGRTMAX; sig++) {
        if (sigaction(sig, &sa, NULL) == -1) {
            perror("Erreur: sigaction");
            exit(1);
        }
    }

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
