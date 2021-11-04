#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void alrmHandler( int sig )
{
	printf("SIGALRM is sent\n" );
}
int main()
{
	signal(SIGALRM, alrmHandler );

	alarm(1);
	while(1){
		int i = 0;
	}
}
