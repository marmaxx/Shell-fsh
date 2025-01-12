#ifndef PIPE_H
#define PIPE_H

int is_Pipe_Command (char *command);
int decoupe_pipe_commande (char *command);
int execute_pipe (char *command, int last_status);

#endif
