#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXUSER 10
#define MAXNAME 10
#define MAXLINE 80

int main (int argc, char *argv[]) {

    int n, listenfd, connfd, caddrlen, fdmax, fdnum, usernum;
    struct hostent *h;
    struct sockaddr_in saddr, caddr;
    char buf[MAXLINE];
    char name[MAXUSER][MAXNAME];
    char temp[MAXLINE+MAXNAME+1];
    int port = atoi(argv[1]);
    fd_set readset, copyset;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket() failed.\n");
        exit(1);
    }

    memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    if (bind(listenfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        printf("bind() failed.\n");
        exit(2);
    }

    if (listen(listenfd, 5) < 0) {
        printf("listen() failed.\n");
        exit(3);
    }

    /* Insert your code for file descriptor set
	*
	*
	**/ 

    while (1) {

        copyset = readset;

        if( /* Insert your code */ < 0 ) {
            printf("select() failed.\n");
            exit(4);
        }

       for (int i = 3; i < fdmax + 1; i++) {

           memset(buf, 0, MAXLINE);
           memset(temp, 0, MAXLINE+MAXNAME+1);

            if ( /* Insert your code */ ) {

                if (i == listenfd) {
                    
                    caddrlen = sizeof(caddr);
                    if ((connfd = accept(listenfd, (struct sockaddr *)&caddr, (socklen_t *)&caddrlen)) < 0) {
                        printf ("accept() failed.\n");
                        continue;
                    }

                    /* Insert your code */ 

                }
                else {

                    /* Insert your code */
                    
                }
            }  
        }
    }

    return 0;

}

