#include <stdio.h> 
#include <stdint.h>
#include <unistd.h>
#include <string.h>


int is_redirection(const char *cmd){
    if(cmd == NULL || strlen(cmd) < 4){
        return 0;
    }
    //recuperation des premieres iteration des redirection
    const char *r_in = strchr(cmd, '<');
    fprintf(stdout, " le r_in est %s \n",r_in);
    const char *r_out = strchr(cmd, '>');
    fprintf(stdout, " le r_out est %s \n",r_out);

    //check s'il y a au moin une redirection 
    if (r_in == NULL && r_out == NULL){
    return 0;
    }

    //check s'il n'y a pas < et > en meme temps 
    if (r_in != NULL && r_out != NULL){
    return 0;
    }

    //check s'il y a bien une espace de chaque cote de < 
    if (r_in != NULL){
        if(*(r_in - 1) != ' ' && *(r_in + 1) != ' ')
        {
            return 0;
        }
    }

    // check les cas pour > 
    if ( r_out != NULL){
        // check juste avant > 
        if (r_out == cmd) {
            // check si > est en début de commande
            return 0;
        } else if (*(r_out - 1) != ' ') {
            // check si avant > n'est pas un espace
            if (!(*(r_out - 1) == '2' && (r_out - 2 >= cmd) && *(r_out - 2) == ' ')) {
                return 0; //erreur si ce n'est pas 2> avrcc un espace avant le 2
            }
        }


        // check juste apres >
        if (*(r_out + 1) != ' ' && 
        !((*(r_out + 1) == '>') && (*(r_out + 2) == ' ')) && 
        !((*(r_out + 1) == '|' ) && (*(r_out + 2) == ' '))){
        return 0;
        }
 
    }

    return 1;
}
