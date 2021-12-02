#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAXLINE 512

const int MAXUSER = 1023;
const int MINUSER = 0;
const int MAXACTION = 5;
const int MINACTION = 0;
const int MAXSEAT = 255;
const int MINSEAT = 0;

enum actions {
    TERMINATE = 0,
    LOGIN,
    RESERVE,
    CHECK_RES,
    CANCEL_RES,
    LOGOUT,
};

typedef struct {
    int user;
    int action;
    int data;
} query;
typedef struct {
    int passcode;
    int seat;
    char signup;
} user;
typedef struct {
    int* seats;
    user* users;
    int connfd;
    pthread_mutex_t lock_connfd;
    pthread_mutex_t lock_seat;
    pthread_mutex_t* lock_users;
} client_arg;

void* client_handler( void* args_void )
{
    client_arg* args = ( (client_arg*)args_void );
    int n;
    query* q;
    int connfd;
    {
        pthread_mutex_lock( &(args->lock_connfd) );
        connfd = args->connfd;
        pthread_mutex_unlock( &(args->lock_connfd) );
    }

    pthread_detach( pthread_self() );
    
    printf( "client_handler!\n" );
    enum actions action;
    q = (query*)malloc( sizeof(query) );
    char logInFlag = 0; // false == 0, true == 1
    while ( ( n = read( connfd, q, sizeof(query) ) ) > 0 ){
        printf( "user: %d, action: %d, data: %d\n", q->user, q->action, q->data ); // Debug
        if ( q->user > MAXUSER || q->user < MINUSER ){
            write( connfd, (int*)-1, sizeof(int) );
            continue;
        }
        if ( q->action > MAXACTION || q->action < MINACTION ){
            write( connfd, (int*)-1, sizeof(int) );
            continue;
        }

        action = (enum actions)(q->action);
        /* Not logged in */
        if ( logInFlag == 0 ){
            if ( action != LOGIN ){
                write( connfd, (int*)-1, sizeof(int) );
                continue;
            }

            if ( pthread_mutex_trylock( &(args->lock_users[q->user]) ) != 0 ){
                write( connfd, (int*)-1, sizeof(int) );
                continue;
            }
            /* sign up & log in*/
            if ( args->users[q->user].signup == -1 ){
                args->users[q->user].passcode = q->data;
                args->users[q->user].signup = 1;
                logInFlag = 1;
                write( connfd, (int*)1, sizeof(int) );
                continue;
            }
            /* log in */
            else {
                if ( args->users[q->user].passcode == q->data ){
                    logInFlag = 1;
                    write( connfd, (int*)1, sizeof(int) );
                }
                else 
                    write( connfd, (int*)1, sizeof(int) );
                continue;
            }
        }
        switch (action)
        {
            case TERMINATE:
            {
                break;
            }
            case LOGIN:
            {
                /* code */
                break;
            }
            case RESERVE:
            {
                break;
            }
            case CHECK_RES:
            {
                break;
            }
            case CANCEL_RES:
            {
                break;
            }
            case LOGOUT:
            {
                break;
            }
        }
    }

    free(q);
    close( connfd );
    return NULL;
}

int main( int argc, char* argv[] )
{
    write( 1, (int*)-1, sizeof(int) );
    return 0;
    if ( argc != 2 ){
        printf( "Usage: %s <port number>\n", argv[0] );
        exit(1);
    }

    int listenfd, caddrlen;
    struct sockaddr_in saddr, caddr;
    int port = atoi(argv[1]);
    
    if ( ( listenfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ){
        printf( "socket() failed.\n" );
        exit(1);
    }

    memset( (char*)&saddr, 0, sizeof(saddr) );
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    if ( bind(listenfd, (struct sockaddr* )&saddr, sizeof(saddr) ) < 0 ){
        printf( "bind() failed.\n" );
        exit(2);
    }
    if ( listen( listenfd, 5 ) < 0 ){
        printf( "listen() failed.\n" );
        exit(3);
    }

    int connfd;
    pthread_t tid;
    int seats[MAXSEAT+1];
    user users[MAXUSER+1];
    memset( seats, -1, sizeof(int)*(MAXSEAT+1) );
    memset( users, -1, sizeof(user)*(MAXUSER+1) );
    client_arg* args = malloc( sizeof(client_arg) );
    {
        args->seats = seats;
        args->users = users;
        pthread_mutex_init( &(args->lock_connfd), NULL );
        pthread_mutex_init( &(args->lock_seat), NULL );
        args->lock_users = malloc( sizeof(pthread_mutex_t)*(MAXUSER+1) );
        for ( int i = 0; i <= MAXUSER; i++ )
            pthread_mutex_init( &( args->lock_users[i] ), NULL );
    }
    while ( 1 ){
        caddrlen = sizeof(caddr);
        if ( ( connfd = accept( listenfd, (struct sockaddr *)&caddr, (socklen_t*)&caddrlen ) ) < 0 ){
            printf( "accept() failed.\n" );
            continue;
        }

        /* Handle New client */
        {
            pthread_mutex_lock( &(args->lock_connfd) );
            args->connfd = connfd;
            pthread_mutex_unlock( &(args->lock_connfd) );
            pthread_create( &tid, NULL, client_handler, (void*)args );
        }
    }

    return 0;
}