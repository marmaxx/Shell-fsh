#include <stdio.h> 
#include <stdint.h>
#include <unistd.h>
#include <string.h>


int is_redirection(const char *cmd){
    return (strchr(cmd, '>') || strchr(cmd, '<')) != NULL ; 
}
