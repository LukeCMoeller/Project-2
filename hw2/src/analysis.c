#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
#include "processing_scheduling.h"

#define FCFS "FCFS"
#define P "P"
#define RR "RR"
#define SJF "SJF"

// Add and comment your analysis code in this function.
// THIS IS NOT FINISHED.
int main(int argc, char **argv) 
{
	if (argc < 3) 
	{
		printf("%s <pcb file> <schedule algorithm> [quantum]\n", argv[0]);
		return EXIT_FAILURE;
	}

	abort();		// REPLACE ME with implementation.
	//set up the dyn_array_t from pcb file
	dyn_array_t *ready_queue; 
	//load the process queue
	ready_queue = load_process_control_blocks(argv[0]); 
	//Process/run the schedule algorithm
	
	//Something to do with quantum

	//free the established dyn_array_t *ready_queue
	free(ready_queue);
	return EXIT_SUCCESS;
}
