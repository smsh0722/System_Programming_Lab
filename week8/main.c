#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void make_tokens(char* cmd, char* arg[], char* arg2[], char* target) {

   // insert your code
   // make tokens which will be used in pipe or redirections
   // you can change the parameters if you want
    char* pch = strchr( cmd, target[0] );
    char* l_cmd = (char*)malloc( sizeof(char)*strlen(cmd) );
    char* r_cmd = (char*)malloc( sizeof(char)*strlen(cmd) );
    strncpy( l_cmd, cmd, pch - cmd );
    strcpy( r_cmd, pch + 1 );

    int i;
    {
        i = 0;
        char* ptok;
        ptok = strtok(l_cmd, " ");
        while (ptok != NULL)
        {
            arg[i] = ptok;
            //printf( "arg: \"%s\"\n", arg[i] ); // Debug
            i++;
            ptok = strtok(NULL, " ");
        }
        arg[i] = NULL;
    }
    {
        i = 0;
        char* ptok;
        ptok = strtok(r_cmd, " ");
        while (ptok != NULL)
        {
            arg2[i] = ptok;
            //printf( "arg2: \"%s\"\n", arg2[i] ); // Debug
            i++;
            ptok = strtok(NULL, " ");
        }
        arg2[i] = NULL;
    }
    return;

}

int main () {

    size_t size;
    char *cmd;
    char *ptr;
    char *arg[128];
    char *arg2[128];
    char path[128];
    char path2[128];
    int child_status;
    int fd[2];
    int fdr;
    
    while(1) {

        int num = 0;
        cmd = NULL;
        getline(&cmd, &size, stdin);
        cmd[strlen(cmd)-1] = '\0';

        if(strcmp(cmd, "quit") == 0) {
            break;
        }

        if(strchr(cmd, '|') != NULL) {
            
            make_tokens(cmd, arg, arg2, "|");
            
            sprintf(path, "/bin/%s", arg[0]);
            sprintf(path2, "/bin/%s", arg2[0]);
            
            // insert your code
            // implement pipe using pipe and dup2 functions
            if (fork() == 0) {
                pipe(fd);
                if ( fork() > 0 ){
                    close(fd[0]);
                    dup2( fd[1], 1 );
                    execv(path, arg);
                }
                else {
                    close(fd[1]);
                    dup2( fd[0], 0 );
                    execv(path2,arg2);
                    exit(0);
                }
                wait(&child_status);
                exit(0);

            }
            else
                wait(&child_status); 

        } 
        else if(strchr(cmd, '>') != NULL) {

            make_tokens(cmd, arg, arg2, ">");
            sprintf(path, "/bin/%s", arg[0]);
            
            // insert your code
            // implement > redirection using dup2
            if (fork() == 0) {
                int dst = open( arg2[0], O_WRONLY | O_CREAT | O_TRUNC , 0664 ); 
                dup2( dst, 1 ); // stdout redirection
                execv(path, arg);
                exit(0);
            }
            else
                wait(&child_status);  
        }
        else if(strchr(cmd, '<') != NULL) {

            make_tokens(cmd, arg, arg2, "<");
            sprintf(path, "/bin/%s", arg[0]);
            
            // insert your code
            // implement < redirection using dup2
            if (fork() == 0) {
                int dst = open( arg2[0], O_RDONLY );
                dup2( dst, 0 ); // stdin redirection
                execv(path, arg);
                exit(0);
            }
            else
                wait(&child_status);  

        }
        else {
        	
        	// mini shell from week5 - do not modify 

            ptr = strtok(cmd, " ");
            while (ptr != NULL) {
                arg[num++] = ptr;
                ptr = strtok(NULL, " ");
            }
            arg[num] = NULL;

            sprintf(path, "/bin/%s", arg[0]);
        
            if (fork() == 0) {
                execv(path, arg);
                exit(0);

            }
            else
                wait(&child_status);            

        }

    }

    wait(0);
    return 0;

}