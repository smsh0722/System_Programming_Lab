#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct thread_data {
	int thread_id;
	int result;
};

int row_size;
int col_size;

/*** Insert your own matrix & vector data, functions ***/
int** my_matrix;
int* my_vector;

void init_matrix()
{
    my_matrix = (int**)malloc( sizeof(int*)*(row_size) );
    for ( int i = 0; i < row_size; i++ )
        my_matrix[i] = (int*)malloc( sizeof(int)*(col_size) );

    printf( " *** Matrix ***\n" );
    for ( int r = 0; r < row_size; r++ ){
        for ( int c = 0; c < col_size; c++ ){
            int rand_val = rand()%10;
            my_matrix[r][c] = rand_val;
            printf( "[ %d ] ", rand_val );
        }
        printf( "\n" );
    }
}
void init_vector()
{
    my_vector = (int*)malloc( sizeof(int)*(col_size) );

    printf( " *** Vector ***\n" );
    for ( int r = 0; r < col_size; r++ ){
        int rand_val = rand()%10;
        my_vector[r] = rand_val;
        printf("[ %d ]\n", rand_val );
    }
}


void *thread_mvm(void *arg)
{
	/*** Insert your code ***/
    struct thread_data* dataPtr = (struct thread_data*)arg;
    int r = dataPtr->thread_id;
    int sum = 0;
    for ( int c = 0; c < col_size; c++ )
        sum += my_matrix[r][c] * my_vector[c];


    dataPtr->result = sum;
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: %s <row> <column>\n", argv[0]);
		exit(1);
	}

	row_size = atoi(argv[1]); // row, thread num
	col_size = atoi(argv[2]);
	pthread_t tid[row_size];	
	struct thread_data t_data[row_size];

	/*** Insert your code ***/
    srand(time(NULL));
    init_matrix();
    init_vector();

    long t;
    for ( t = 0; t < row_size; t++ ){
        t_data[t].thread_id = t;
        if ( pthread_create(&tid[t], NULL, thread_mvm, (void*)&t_data[t] ) ){
            printf( "failed to create thread!\n" );
            exit(1);
        }
    }
    
    for ( t = 0; t < row_size; t++ )
        pthread_join(tid[t], NULL );
    
    printf( "\n *** Result ***\n" );
    for ( t = 0; t < row_size; t++ ){
        printf("[ %d ]\n", t_data[t].result );
    }
    
	return 0;
}
