#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char** argv)
{
	int rf;
	if ( (rf = open( argv[1], O_RDONLY ) ) < 0 ){
		perror("open");
		exit(1);
	}

	int wf;
	{
		char fName[100];
		{
			char* pch;
			pch = strchr( argv[1], '.' ); //~.txt
			int pos = pch - argv[1];
			
			strncpy( fName, argv[1], pos );
			strcat( fName, "_num.txt" );
		}
		if ( (wf = open( fName, O_CREAT | O_WRONLY, 0664 )) < 0 ){
			perror("open");
			exit(1);
		}
	}

	struct stat st;
	stat( argv[1], &st );
	int lineN = 1;
	char s[] = " | ";
	char c;
	int bytes = 1;
	do {
		
		if ( bytes > st.st_size ) {
			break;
		}
		bytes++;
		
		if ( c == '\n' || lineN == 1 ){
			int save = lineN;
			int num[9];
			int idx = 0;
			while ( lineN != 0 ){
				num[idx] = lineN % 10;
				lineN /= 10;
				idx++;
			}
			lineN = save;
			while ( idx > 0 ){
				char tmp = num[idx-1] + '0';
				write( wf, &tmp, 1 ); 
				idx--;
			}
			write( wf, s, strlen(s) );
			lineN++;
		}
		else {
			write( wf, &c, 1 );
		}
	} while ( read( rf, &c, 1 ) != 0 );

	close(rf);
	close(wf);

	return 0;
}
