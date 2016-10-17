/*****************************************************
 * FILE: producer-consumer.c
 * DESCRIPTION:
 * O programa implementa uma versão simplificada do
 * problema Produtor-Consumidor. Um argumento é passado
 * pela linha de comando informando o número máximo de
 * iterações entre produtores e consumidores. Estaticamente
 * são definidos os numeros de produtores (2) e consumidores (3).
 * AUTHOR: Thiago Nascimento Rodrigues
 * LAST REVISED: 06/02/15
 ****************************************************/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

#define BUFFER_SIZE 20
#define MAX_RANDOM 100
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 3

int main (int argc, char* argv[]) {

	int buffer[BUFFER_SIZE], limit_iterations, occupancy_rate = 0;
	register int i;
	double parallel_start, parallel_end;

	limit_iterations = atoi(argv[1]);
	
	if (!omp_in_parallel()) printf("[DEBUG] Serial program execution has been started.\n");
	printf("[DEBUG] Number of available processors: %d\n", omp_get_num_procs());
	
	omp_set_nested(1);
	if (omp_get_nested()) printf("[DEBUG] Nested threads has been enabled.\n");


	// Initializing buffer
	for (i = 0; i < BUFFER_SIZE; buffer[i++] = -1);
	
	omp_set_num_threads(NUM_PRODUCERS + NUM_CONSUMERS);
	printf("Max number of producers/consumers has been setted: %d\n", omp_get_max_threads());

	
	// Randomic seed
	srand(time(NULL));
	
	parallel_start = omp_get_wtime();

	#pragma omp parallel num_threads(2)
	{
		#pragma omp single
		if (omp_in_parallel()) printf("[DEBUG] Parallel program execution has been started.\n");

		int tid = omp_get_thread_num();
		int iteration = 0;

		#pragma omp sections 
		{
			#pragma omp section  
			{
				#pragma omp parallel num_threads(NUM_PRODUCERS) 
				{
					#pragma omp single
					printf("Number of producers started: %d\n", omp_get_num_threads());

					while (iteration < limit_iterations) {
						
						// Waiting free buffer
						while (occupancy_rate == BUFFER_SIZE) {
							printf("Producer id = [%d/%d] waiting buffer to free ...\n", omp_get_thread_num(), tid);
							sleep(0.5);
						}

						int position = rand() % BUFFER_SIZE;
						int product = rand() % MAX_RANDOM;

						#pragma omp critical
						{
							if (buffer[position] == -1) {
								buffer[position] = product;
								printf("Producer id = [%d/%d] has produced %d at position %d (Occupancy rate: %d).\n", omp_get_thread_num(), tid, buffer[position], position, ++occupancy_rate);

							}
							else {
								printf("Producer id = [%d/%d] has failed to produce at position %d.\n", 
										omp_get_thread_num(), tid, position);
							}
						}

						#pragma omp atomic
						iteration++;
					}
				}

			}

			#pragma omp section
			{
				#pragma omp parallel num_threads(NUM_CONSUMERS)
				{
					#pragma omp single
					printf("Number of consumers started: %d\n", omp_get_num_threads());

					while (iteration < limit_iterations) {
						
						// Waiting fill buffer
						while (occupancy_rate == 0) {
							printf("Consumer id = [%d/%d] waiting buffer to fill ...\n", omp_get_thread_num(), tid);
							sleep(0.5);
						}

						int position = rand() % BUFFER_SIZE;

						#pragma omp critical
						{
							if (buffer[position] != -1) {
								printf("Consumer id = [%d/%d] has consumed %d at position %d (Occupancy rate: %d).\n", omp_get_thread_num(), tid, buffer[position], position, --occupancy_rate);
								buffer[position] = -1;
							}
							else {
								printf("Consumer id = [%d/%d] has failed to consume at position %d.\n", 
										omp_get_thread_num(), tid, position);
							}
						}

						iteration++;
					}
				}
			}
		}
		
	}

	parallel_end = omp_get_wtime();

	if (!omp_in_parallel()) printf("[DEBUG] Serial program execution has been started.\n");

	printf("[DEBUG] Execution time of parallel section: %lf.\n", parallel_end - parallel_start);

	omp_set_nested(0);

	if (!omp_get_nested()) printf("[DEBUG] Nested threads has been disabled.\n");


	printf("\nBuffer Final Status: ");
	for (i = 0; i < BUFFER_SIZE; printf("%d ", buffer[i++]));
	printf("\n");

	return 0;
}
