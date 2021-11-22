#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int num_of_points_inside_circle;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *thread_montecarlo( void* arg )
{
    int num_of_points_per_thread = *( (int*) arg );
	int point_in_circle = 0;
    int radius = 1;

    double x, y, rst;
    for ( int i = 0; i < num_of_points_per_thread; i++ ){
        x = rand() / (double)RAND_MAX;
        y = rand() / (double)RAND_MAX;
        rst = x*x + y*y;
        if ( rst < radius )
            point_in_circle++;
    }
    
    /* mutex */
    {
        pthread_mutex_lock(&m);
        num_of_points_inside_circle += point_in_circle;
        pthread_mutex_unlock(&m);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

	if (argc != 3) {
		printf("Usage: %s <# of threads> <# of points per thread>\n", argv[0]);
		exit(1);
	}

	int num_of_thread = atoi(argv[1]);
    int num_of_points_per_thread = atoi(argv[2]);
    int num_of_points_total = num_of_thread * num_of_points_per_thread;
    num_of_points_inside_circle = 0;

	pthread_t tid[num_of_thread];

    srand( time(NULL) );
    long t;
    for ( t = 0; t < num_of_thread; t++ ){
        if ( pthread_create( &tid[t], NULL, thread_montecarlo, (void*)&num_of_points_per_thread ) ){
            printf( "failed to create thread!\n" );
            exit(1);
        }
    }

    for ( t = 0; t < num_of_thread; t++ )
        pthread_join(tid[t], NULL );
    
    pthread_mutex_destroy(&m);

    double rst_pi = 4 * ( num_of_points_inside_circle / (double) num_of_points_total );
    printf( "%f\n", rst_pi );

	return 0;
}

