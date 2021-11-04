#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAXNAME 51
#define MAXFILE 200001
#define MAXCOMM 256

int main( int argc, char* argv[] )
{
    int n, listenfd, connfd, caddrlen;
    struct sockaddr_in saddr, caddr;

    int port = atoi( argv[1] );

    if ( (listenfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
        printf( "socket() failed.\n");
        exit(1);
    }
    memset( (char*)&saddr, 0, sizeof( saddr) );
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    saddr.sin_port = htons( port );
    if ( bind(listenfd, (struct sockaddr* )&saddr, sizeof(saddr) ) < 0 ){
        printf( "bind() failed.\n");
        exit(2);
    }
    if( listen( listenfd, 5 ) < 0 ){
        printf( "listen() failed.\n");
        exit(3);
    }
    while ( 1 ){
        caddrlen = sizeof( caddr );
        if ( ( connfd = accept( listenfd, (struct sockaddr*)&caddr, (socklen_t*)&caddrlen ) ) < 0 ){
            printf( "accept() failed.\n" );
            continue;
        }

        char buf_name[MAXNAME];
        char buf_file[MAXFILE];
        char buf_comm[MAXCOMM];

        {
            // read file name from client
            while ( ( n = read( connfd, buf_name, MAXNAME ) ) > 0 ){
                // write name to client
                write( connfd, buf_name, n );

                // read file from client
                if ( ( n = read( connfd, buf_file, MAXFILE ) ) <= 0 )
                    continue;
                sprintf( buf_comm, "%d", n );
                write( 1, "got ", sizeof(char)*strlen("got ") );
                write( 1, buf_comm, sizeof(char)*strlen(buf_comm) );
                write( 1, " bytes from client.\n", sizeof(char)*strlen( " bytes from client.\n") );
                // write result to client
                write( connfd, buf_comm, strlen(buf_comm) );
                
                // copy
                char fileName[128] = {0};
                {
                    strcpy( fileName, buf_name );
                    strcat( fileName, "_copy");
                }
                int fd = open( fileName, O_CREAT | O_WRONLY, 0664 );
                write( fd, buf_file, n );
                close(fd);
            }
            close( connfd );
            
            return 0;
        }
    }

    return 0;
}