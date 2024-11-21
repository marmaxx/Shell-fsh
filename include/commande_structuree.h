#ifndef COMMANDE_STRUCTUREE_H
#define COMMANDE_STRUCTUREE_H

int is_structured(const char *command);
char **decoupe_commande_structuree (const char *command);
int execute_structured_command(const char *command, int last_status);

#endif 