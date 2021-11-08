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



void *thread_mvm(void *arg) {
	/*** Insert your code ***/

}

int main(int argc, char *argv[]) {

	if (argc != 3) {
		printf("Usage: %s <row> <column>\n", argv[0]);
		exit(1);
	}

	row_size = atoi(argv[1]);
	col_size = atoi(argv[2]);
	pthread_t tid[row_size];	
	struct thread_data t_data[row_size];

	/*** Insert your code ***/

	return 0;
}

