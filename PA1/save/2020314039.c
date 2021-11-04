#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

typedef enum{
	WORD,
	WORDS,
	PHRASE,
	REGEX
} caseEnum;

typedef struct keyword {
	int capacity;
	int len;
	char* buf;
}keyword;
	
void readL( keyword* key );
caseEnum caseSearch( keyword* key );
void print_i2str( int x );
char to_lower( const char c );
void searchA( char* buf, const int BUFSIZE, keyword* key, caseEnum keyCase );
void searchB( char* buf, const int BUFSIZE, keyword* key ); // WORDS
void searchC( char* buf, const int BUFSIZE, keyword* key );
int split( char* s, int idx, int len, char c );

int main( int argc, char** argv )
{
	
	int fd;
	if ( (fd = open( argv[1], O_RDONLY ) ) < 0 ){
		write( 1, "open", sizeof(char) * 4 );
		exit(1);
	}
	
	char* buf;
	int size;
	{
		struct stat st;
		stat( argv[1], &st );
		size = st.st_size;
		buf = (char*)malloc( sizeof(char)*size );
		if( ( fd = read(fd, buf, sizeof(char)*size ) ) < 0 ){
			write( 1, "read", sizeof(char) * 4 );
			exit(1);
		}
	}
	close( fd );

	caseEnum keyCase;
	while ( 1 ){
		keyword key;
		readL( &key );
		keyCase = caseSearch(&key);
		if ( keyCase == WORD || keyCase == PHRASE ){
			searchA( buf, size, &key, keyCase );
		}
		else if ( keyCase == WORDS ){
			searchB( buf, size, &key );	
		}
		else if ( keyCase == REGEX ){
			searchC( buf, size, &key );
		}
		free( key.buf );
	}
	return 0;
}

void readL( keyword* key )
{
	int i = 0;
	key->capacity = 512;
	key->buf = malloc(sizeof(char) * key->capacity );
	while ( 1 ){
		read( 0, (key->buf + i), sizeof(char) );
		if ( key->buf[i] == '\n' ){
			break;
		}
		else{
			if ( key->capacity == ( i + 1 ) ){
				key-> capacity *= 2;
				key->buf = realloc( key->buf, key->capacity * sizeof(char) );
			}
			i++;
		}
	}
	key->len = i + 1;
}

caseEnum caseSearch( keyword* key )
{
	caseEnum keyCase = WORD;
	for ( int i = 0; i < key->len; i++ ){
		if ( key->buf[i] == ' ' ){
			keyCase = WORDS;
			break;
		}
		else if ( key->buf[i] == '\"' ){
			keyCase = PHRASE;
			break;
		}
		else if ( key->buf[i] == '*' ){
			keyCase = REGEX;
			break;
		}
	}

	return keyCase;
}

void print_i2str( int x )
{
	char num_r[10];
	int i = 0;
	do {
		num_r[i] = (x % 10) + '0';
		x /= 10;
		i++;
	} while ( x > 0 );
	
	for ( int idx = i - 1; idx >= 0; idx-- ){
		write( 1, (num_r+idx), sizeof(char) );
	}
}

char to_lower( const char c )
{
	if ( c >= 'A' && c <= 'Z' ){
		return ( (int)c + ('a' - 'A') );
	}
	else {
		return c;
	}
}

void searchA( char* buf, const int BUFSIZE, keyword* key, caseEnum keyCase )
{
	int rear, front, ki; // rearIdx, frontIdx, keyIdx
	int lNum, bi, li; // lineNum, bufIdx, lineIdx
	
	if ( keyCase == WORD ){
		front = 0;
		rear = (key->len - 1) - 1;
	}
	else { //keyCase == PHRASE;
		front = 1;
		rear = (key->len - 1) - 2;
	}
	
	lNum = 0;
	bi = 0;
	char spaceF = 'F'; // space flag
	while ( bi < BUFSIZE ){
		lNum++;
		li = bi;
		ki = front;

		while ( bi < BUFSIZE && buf[bi] != '\n' ){
			// a char check
			ki = to_lower( key->buf[ki] ) == to_lower( buf[bi++] ) ? ( ki + 1 ) : front;

			// complete check
			if ( ki > rear ){
				if ( (bi == BUFSIZE )||( (bi<BUFSIZE)&&(buf[bi] == '\n' || buf[bi] == ' ' || buf[bi] == '\t' ) ) ){
					int si = (bi - 1 - li) - (rear - front);
					int flag = 0;
					if ( si == 0 ) flag = 1;
					else if ( si > 0 ){
						int pi = si + li - 1;
						if ( buf[pi] == ' ' || buf[pi] == '\t' ) flag = 1;
					}
					
					if ( flag == 1 ){
						if ( spaceF == 'T' ) write( 1, " ", sizeof(char) ); 	// space
						print_i2str( lNum ); 					// line
						write( 1, ":", sizeof(char) );			// :
						print_i2str( si );					// start idx
						
						spaceF = 'T'; // flag set
					}
				}
				ki = front; // reset t, f both case
			}
		}
		bi++; // new line's first idx
	}
	write( 1, "\n", sizeof(char) ); // End with new Line
}

int split( char* s, int idx, int len, char c )
{
	for ( int i = 0; i < len; i++ ){
		if ( s[idx+i] == c ){
			return idx+i;
		}
	}
	return -1;
}

void searchB( char* buf, const int BUFSIZE, keyword* key )
{
	int kf, kr;
	int li, bi, bf, br, bl, lineN;
	
	lineN = 0;
	bi = 0;
	char spaceF = 'F';
	while ( bi < BUFSIZE ){
		lineN++;
		li = bi;
		
		kf = 0;
		kr = split( key->buf, kf, key->len, ' ' );

		while ( bi < BUFSIZE && buf[bi] != '\n' ){
			bi++;
		}
		bl = bi - li + 1;
		bf = li;
		{
			int t1 = split( buf, bf, bl, ' ' );
			if ( t1 == -1 ) t1 = bi;
		        int t2 = split( buf, bf, bl, '\t' );
			if ( t2 == -1 ) t2 = bi;
			br = t1 < t2 ? t1 : t2;       
		}
		
		int flag = 1; // -1:False, 0: , 1:loop, 2:done
		while ( flag == 1 ){
			if ( br - bf == kr - kf ){
				int i;
				for ( i = 0; i < kr - kf; i++ ){
					if ( to_lower( buf[bf+i] ) != to_lower( key->buf[kf+i] ) ){
						i = -1;
						break;
					}
				}

				if ( i >= kr - kf ){
					bf = li;
					{ 
						int t1 = split( buf, bf, bl, ' ');
						if ( t1 == -1 ) t1 = bi;
						int t2 = split( buf, bf, bl, '\t' );
						if ( t2 == -1 ) t2 = bi;
						br = t1 < t2 ? t1 : t2;
					}

					kf = kr + 1;
					if ( kf >= key->len ){
						flag = 2;
						break;
					}
					kr = split( key->buf, kf, key->len - kf, ' ');
					if ( kr == -1 ) kr = key->len - 1;
				}
				else {
					flag = 0;
				}

			}
			else flag = 0;

			if ( flag == 0 ) {
				bf = br + 1;
				if ( bf >= bi ) {
					flag = -1;
					break;
				}
				{
					int t1 = split( buf, bf, bl - (bf-li), ' ' );
					if ( t1 == -1 ) t1 = bi;
					int t2 = split( buf, bf, bl - (bf-li), '\t' );
					if ( t2 == -1 ) t2 = bi;
					br = t1 < t2 ? t1 : t2;
				}

				flag = 1;
			}
		}
		if ( flag == 2 ){
			if ( spaceF == 'T' ) write( 1, " ", sizeof(char) );
			print_i2str( lineN );
			spaceF = 'T';
		}

		bi++;
	}
	write( 1, "\n", sizeof(char) );
}

void searchC( char* buf, const int BUFSIZE, keyword* key )
{
	int kf, kr;
	int li, bi, bf, br, bl, lineN;
	
	lineN = 0;
	bi = 0;
	char spaceF = 'F';
	while ( bi < BUFSIZE ){
		lineN++;
		li = bi;
		
		kf = 0;
		kr = split( key->buf, kf, key->len, '*' );

		while ( bi < BUFSIZE && buf[bi] != '\n' ){
			bi++;
		}
		bl = bi - li + 1;
		bf = li;
		{
			int t1 = split( buf, bf, bl, ' ' );
			if ( t1 == -1 ) t1 = bi;
		        int t2 = split( buf, bf, bl, '\t' );
			if ( t2 == -1 ) t2 = bi;
			br = t1 < t2 ? t1 : t2;       
		}
		
		int flag = 0; // 0: false, 1: key1, 2: btw, 3:True
		while ( bf < bi ){
			if ( br - bf == kr - kf ){
				int i = 0;
				for ( i = 0; i < kr - kf; i++ ){
					if ( to_lower(buf[bf+i] ) != to_lower(key->buf[kf+i]) ){
						break;
					}
				}
				if ( i >= kr - kf ){
					flag = 1;
					break;
				}
			}
			
			bf = br + 1;
			if ( bf >= bi ) break;
			{
				int t1 = split( buf, bf, bl - (bf - li), ' ' );
				if ( t1 == -1 ) t1 = bi;
				int t2 = split( buf, bf, bl - (bf - li), '\t' );
				if ( t2 == -1 ) t2 = bi;
				br = t1 < t2 ? t1 : t2;
			}
		}
		if ( flag == 1 ){
			while ( bf < bi ){
				bf = br + 1;
				if ( bf >= bi ) break;
				{
					int t1 = split( buf, bf, bl - (bf - li), ' ' );
					if ( t1 == -1 ) t1 = bi;
					int t2 = split( buf, bf, bl - (bf - li), '\t' );
					if ( t2 == -1 ) t2 = bi;
					br = t1 < t2 ? t1 : t2;
				}
				
				if ( br - bf >= 1 ) {
					flag = 2;
					break;
				}
			}
		}
		if ( flag == 2 ){
			kf = kr + 1;
			kr = key->len - 1;
			while ( bf < bi ){
				bf = br + 1;
				if ( bf >= bi ) break;
				{
					int t1 = split( buf, bf, bl - (bf - li), ' ' );
					if ( t1 == -1 ) t1 = bi;
					int t2 = split( buf, bf, bl - (bf - li), '\t' );
					if ( t2 == -1 ) t2 = bi;
					br = t1 < t2 ? t1 : t2;
				}
				if ( br - bf == kr - kf ){
					int i = 0;
					for ( i = 0; i < kr - kf; i++ ){
						if ( to_lower(buf[bf+i] ) != to_lower(key->buf[kf+i]) ){
							break;
						}
					}
					if ( i >= kr - kf ){
						flag = 3;
						break;
					}
				}
			}
		}
		if ( flag == 3 ){
			if ( spaceF == 'T' ) write( 1, " ", sizeof(char) );
			print_i2str( lineN );
			spaceF = 'T';
		}
		
		bi++;
	}
	write( 1, "\n", sizeof(char) );
}
