#ifndef BOUCLE_FOR_H
#define BOUCLE_FOR_H 

int boucle_for (char ** args, int last_status);
void concatenate_args(char *args[], char *result);
int exec_interieur_for(struct dirent *entry, int last_status,int option_p, int option_r, int option_e, int option_t, int option_A, int size, char *ext, char *type, char *rep, char **commande, char **args);

#endif 
