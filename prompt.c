#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

#include "pwd.h"

#define GREEN "\001\033[32m\002"  
#define RED "\001\033[91m\002"   
#define BLUE "\001\033[34m\002"  
#define CYAN "\001\033[36m\002"   
#define RESET "\001\033[00m\002"  // Retour à la couleur normale

// Fonction pour créer un prompt tronqué
void create_prompt(int last_status, char *prompt, size_t size) {   
    // Récupération du chemin absolu grâce à la fonction créée dans pwd.c
    char *cwd = chemin_absolu();

    // Déterminer la couleur en fonction du dernier statut
    const char *status_color = (last_status == 0) ? GREEN : RED;
    const char *dir_color = (last_status == 0) ? BLUE : CYAN;

    // Format de retour de la commande : [valeur] ou [SIG]
    char status[8];
    if (last_status == -1) {
        snprintf(status, sizeof(status), "SIG");
    } else {
        snprintf(status, sizeof(status), "%d", last_status);
    }

    // Tronquer le chemin 
    size_t max_dir_length = 30;
    char truncated_cwd[PATH_MAX];

    if (strlen(cwd) > max_dir_length) {
        snprintf(truncated_cwd, sizeof(truncated_cwd), "...%s", cwd + strlen(cwd) - max_dir_length + 3);
    } else {
        snprintf(truncated_cwd, sizeof(truncated_cwd), "%s", cwd);
    }

    snprintf(prompt, size, "%s[%s]%s%s%s$ ", status_color, status, dir_color, truncated_cwd, RESET);

    free(cwd);
}
