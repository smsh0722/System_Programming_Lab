#include <stdio.h>

int plus( int a, int b );
int minus( int a, int b );

int main( void )
{
	int a, b, r, s;

	scanf( "%d %d", &a, &b );
	r = plus( a, b );
	s = minus( a, b );
	printf( "%d %d\n", r, s );

	return 0;
}
