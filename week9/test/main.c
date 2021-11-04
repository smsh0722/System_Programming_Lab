#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>
#include <time.h> 


struct msg
{
	long msgtype;
	/* implement here */ 
	char text[256];
};


struct msg_ack 
{
	long msgtype;
	/* implement here */ 
	char read_time[256];
};


int main()
{
	/* 
	 * @ int uid: user id 
	 * @ int receiver_id: receveri's id 
	 * */ 
	int uid; 
	int receiver_id; 
	printf("my id is\n" );
	scanf("%d", &uid);
	getchar(); // flush   
	printf( "who to send ?\n" );
	scanf("%d", &receiver_id); 
	getchar(); // flush  
		
	/* 
	 * code to get key and QID 
	 *
	 *
	 * */ 
	key_t uk = ftok(".", uid );
	key_t rk = ftok(".", receiver_id );
	int u_qid = msgget( uk, IPC_CREAT | 0660 );
	if ( u_qid == -1 ){
		perror("msgget error: " );
		exit(0);
	}
	int r_qid = msgget( rk, IPC_CREAT | 0660 );
	if ( r_qid == -1 ){
		perror("msgget error: " );
		exit(0);
	}

	struct msg_ack* rm_ack = (struct msg_ack*)malloc( sizeof(struct msg_ack ) );
	struct msg* rm_norm = (struct msg*)malloc( sizeof(struct msg) );
	struct msg_ack* m_ack = (struct msg_ack*)malloc( sizeof(struct msg_ack ) );

	struct msg* m_norm = (struct msg*)malloc( sizeof(struct msg) );

	if (fork() == 0) 
	{
		while (1)
		{
			/* receive message  */ 
			/* get msg from queue and print out */
			if ( msgrcv( u_qid, (void*)rm_ack, sizeof(struct msg_ack), 2, IPC_NOWAIT ) != -1 ){
				printf( "%d read message at:\t%s\n", receiver_id, rm_ack->read_time );
			}

			if ( msgrcv( u_qid, (void*)rm_norm, sizeof(struct msg), 1, IPC_NOWAIT ) != -1 ){
				printf("RECEIVED %s\n", rm_norm->text );
				
				{
					m_ack->msgtype = 2;
					{
						time_t rawtime;
						struct tm * timeinfo;
						time ( &rawtime );
						timeinfo = localtime ( &rawtime );
						strcpy( m_ack->read_time, asctime(timeinfo) );	
					}
					if( msgsnd( r_qid, (void*)m_ack, sizeof(struct msg_ack), 0) == -1 ){
						perror( "msgsnd error : " );
						exit(0);
					}
				}
			}
		}		
	}
	else
	{
		while (1) 
		{
			/*0send message  */ 
			/* get user input and send to the msg queue*/
			m_norm->msgtype = 1;
			char* text = NULL;
			{
				size_t size = 0;
				getline( &text, &size, stdin );
				fflush(stdin);
			}
			strcpy( m_norm->text, text );
			if( msgsnd( r_qid, (void*)m_norm, sizeof(struct msg), 0 ) == -1 ){
				perror( "msgsnd error : " );
				exit(0);
			}
		}
	}	

	free(rm_ack);
	free(rm_norm);
	free(m_ack);
	free(m_norm);
	return 0;
}