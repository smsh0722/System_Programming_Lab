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
    int n, cfd;
    struct hostent* h;
    struct sockaddr_in saddr;
    char buf_name[MAXNAME];
    char buf_file[MAXFILE];
    char buf_comm[MAXCOMM];
    char* host = argv[1];
    int port = atoi( argv[2] );

    if ( ( cfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
        printf( "socket() failed.\n");
        exit(1);
    }
    if ( (h = gethostbyname(host) ) == NULL ){
        printf( "invalid hostname %s\n", host );
        exit(2);
    }
    memset( (char*)&saddr, 0, sizeof( saddr) );
    saddr.sin_family = AF_INET;
    memcpy( (char*)&saddr.sin_addr.s_addr, (char*)h->h_addr, h->h_length );
    saddr.sin_port = htons( port );

    if( connect( cfd, (struct sockaddr*)&saddr, sizeof(saddr) ) < 0 ){
        printf( "connect() failed.\n" );
        exit(3);
    }

    while ( ( n = read( 0, buf_name, MAXNAME) ) > 0 ){
        buf_name[n-1] = 0;
        if ( strcmp( buf_name, "quit" ) == 0 )
            break;
        
        // write file name to server
        write( cfd, buf_name, n );
        // read file name from server
        {
            if ( ( n = read( cfd, buf_comm, MAXCOMM ) ) <= 0 )
                continue;
            write( 1, "FILE NAME : ", sizeof(char) * strlen( "FILE NAME : " ) );
            write( 1, buf_comm, n );
            write( 1, "\n", sizeof( char ) * 1 );
        }

        // write file to server
        {
            int fd;
            if ( (fd = open( buf_name, O_RDONLY ) ) < 0 ){
                printf( "file is not exist.\n" );
                continue;
            }
            struct stat st;
            stat( buf_name, &st );
            n = read( fd, buf_file, st.st_size );
            close(fd);
            
            write( cfd, buf_file, n );
        }
        // read result from server
        {
            if ( ( n = read( cfd, buf_comm, MAXCOMM ) ) <= 0 )
                continue;
            write( 1 , "Send ", sizeof(char)*strlen("Send ") );
            write( 1, buf_comm, sizeof(char)*n);
            write( 1, " bytes to server.\n", sizeof(char)*strlen(" bytes to server.\n") );
        }
    }
    close(cfd);

    return 0;
}