#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TOK_SIZE 10

void runShell( const char path[], char* const  args[] )
{
	if ( fork() == 0 ){
		execv( path, args );
		exit(0);
	}
	wait(NULL);
}

int main( void )
{
	while( 1 ) {
		char* cmd;
		size_t size;
		getline(&cmd, &size, stdin);
		cmd[strlen(cmd)-1] = '\0';

		if ( strcmp(cmd,"quit") == 0 ){
			break;
		}
		else {
			char* mytok[TOK_SIZE];
			{
				int i = 0;
				char* pch = strtok( cmd, " " );
				while( pch != NULL ){
					mytok[i++] = pch;
					pch = strtok( NULL, " " );
				}
				mytok[i] = NULL;
			}
			
			char path[100];
			sprintf( path, "/bin/%s", mytok[0] );
			
			runShell( path, mytok );
		}
	}
	
	exit(0);
}
