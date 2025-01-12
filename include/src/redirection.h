#ifndef REDIRECTION_H
#define REDIRECTION_H

int is_redirection(const char *cmd);
int is_simple_redirection(const char *cmd);
int make_redirection(char *cmd,int last_status);

#endif