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
const int MAXID = 5;
const int MINID = 0;
const int MAXSEAT = 255;
const int MINSEAT = 0;

typedef struct {
    int user;
    int action;
    int data;
} query;

typedef struct {
    int passcode;
    int seat;
} user;

int* seats;
user* users;

void* client_handler( void* arg )
{
    int n;
    query* q;
    int connfd = *( (int*)arg );

    pthread_detach( pthread_self() );
    free(arg);
    
    printf( "client_handler!\n" );
    q = (query*)malloc( sizeof(query) );
    while ( ( n = read(connfd, q, sizeof(query) ) ) > 0 ){
        printf( "user: %d, action: %d, data: %d\n", q->user, q->action, q->data ); // Debug
    }

    free(q);
    close(connfd);
    return NULL;
}

int main( int argc, char* argv[] )
{
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

    int* connfdp;
    pthread_t tid;
    seats = (int*)malloc( sizeof(int)*(MAXSEAT + 1) );
    users = (user*)malloc( sizeof(user)*(MAXUSER + 1 ) );
    memset( seats, -1, sizeof(int)*(MAXSEAT + 1) );
    memset( users, -1, sizeof(*users)*(MAXUSER + 1) );
    /*
    { // Debug
        for ( int i = 0; i <= MAXSEAT; i++ )
            printf( "%d\n", seats[i] );
        for ( int i = 0; i <= MAXUSER; i++ )
            printf( "[%d] passcode: %d, seat: %d\n", i, users[i].passcode, users[i].seat );
        return 0;
    }
    */
    while ( 1 ){
        connfdp = (int*)malloc(sizeof(int));
        if ( ( *connfdp = accept( listenfd, (struct sockaddr*)&caddr, (socklen_t*)&caddrlen ) ) < 0 ){
            printf( "accept() failed.\n" );
            free(connfdp);
            continue;
        }

        pthread_create( &tid, NULL, client_handler, (void*)connfdp );
    }
    
    free( seats );
    free( users );

    return 0;
}