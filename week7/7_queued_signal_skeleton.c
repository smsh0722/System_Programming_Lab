#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int num_sending_signal;
int num_recv_signal=0;
int num_recv_ack = 0;
pid_t pid;

void recv_ack_handler(int sig){
	
	// insert your code
	
	return;
}

void terminate_handler(int sig){
	
	// insert your code
	
	exit(0);
}

void sending_handler(int sig){	
	
	// insert your code
	
	return;
}

void sending_ack(int sig){
	
	// insert your code 
	
	return;
}

int main(int argc, char *argv[]){
	num_sending_signal = atoi(argv[1]);
	printf("sending signal: %d\n", num_sending_signal);
	pid = fork();
	if (pid == 0){
		pid = getppid();
		
		// insert your code
		
		// signal using sending_ack
		// signal using terminate_handler
		
		while(1){
			int i = 0;
		}
	}
	else{
		
		// insert your code
		
		// signal using recv_ack_handler
		// signal using sending_handler
		
		while(1){
			int i = 0;
		}
	}
	return 0;
}
