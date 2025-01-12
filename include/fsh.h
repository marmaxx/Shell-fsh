#ifndef FSH_H
#define FSH_H

int execute_commande_quelconque(char **args, int last_status);
int main();
void ignore_SIGTERM (int signum);

#endif