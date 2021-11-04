#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

int check_min( char* minute, struct tm* tm );

int main(void)
{
	unsigned int pid;
	time_t t;
	struct tm *tm;	
	int fd;
	char *argv[3];
	char buf[512];
	int fd0, fd1, fd2;

	fd = open("./crontab", O_RDWR);
	pid = fork();
	
	if(pid == -1) return -1;
	if(pid != 0)
		exit(0);
	if(setsid() < 0)
		exit(0);
	if(chdir("/") < 0)
		exit(0);

	umask(0);

	close(0);
	close(1);
	close(2);

	fd0 = open("/dev/null", O_RDWR);
	fd1 = open("/dev/null", O_RDWR);
	fd2 = open("/dev/null", O_RDWR);

	t = time(NULL);
	tm = localtime(&t);
	
	while (1)
	{
		memset( buf, 0, sizeof(buf) );
		for ( int i = 0; i < 3; i++ ) argv[i] = "";

		// insert your code
		if ( fork() == 0 ){
			lseek(fd, 0, SEEK_SET);
			read(fd, buf, sizeof(buf) );
			{
				char* pos = buf;
				argv[0] = strtok_r( pos, " ", &pos ); // minute 0~59
				argv[1] = strtok_r( pos, " ", &pos ); // hour 0~23
				argv[2] = strtok_r( pos, " ", &pos ); // path
			}

			t = time(NULL);
                       tm = localtime(&t);
			if ( strcmp( argv[1], "*" ) == 0 ){
				if ( check_min( argv[0], tm ) != 0 ){
					execl( "/bin/sh", "/bin/sh", "-c", argv[2], (char*) NULL );
				}
			}
			else if ( atoi(argv[1]) == tm->tm_hour ){
				if ( check_min( argv[0], tm ) != 0){
					execl( "/bin/sh", "/bin/sh", "-c", argv[2], (char*) NULL );
				}
			}
			exit(0);
		}	

		t = time(NULL);
		tm = localtime(&t);
	
		sleep(60 - tm->tm_sec % 60);
	}
	wait(0);
	return 0;
}

int check_min( char* minute, struct tm* tm )
{
	if ( strcmp( minute, "*" ) == 0 ){
		return 1; // true
	}
	else if ( atoi(minute) == tm->tm_min ){
		return 1; // true
	}
	else return 0; // false
}
