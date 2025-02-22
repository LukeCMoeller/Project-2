#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
#include "processing_scheduling.h"

#define FCFS "FCFS"
#define P "P"
#define RR "RR"
#define SJF "SJF"
#define SRT "srt"
int main(int argc, char **argv) 
{
	    if (argc < 3) 
	    {
	        printf("%s <pcb file> <schedule algorithm> [quantum]\n", argv[0]);
	        return EXIT_FAILURE;
	    }
	
	    //load process control blocks
	    dyn_array_t *ready_queue = load_process_control_blocks(argv[0]);
	    if (!ready_queue)
	    {
	        printf("Process Failed.\n");
	        return EXIT_FAILURE;
	    }
	
	    ScheduleResult_t result = {0}; //results 
	    int algorithm_check = 0; //flag for valid algorithm
	
	    
	    char *algorithm = argv[2]; //get command line inputes
		//execute algorithm FCFS
	    if (algorithm[0] == 'F' && algorithm[1] == 'C' && algorithm[2] == 'F' && algorithm[3] == 'S' && algorithm[4] == '\0') 
	    {
	        algorithm_check = first_come_first_serve(ready_queue, &result);
	    }
	    else if (algorithm[0] == 'P' && algorithm[1] == '\0') //execute priority
	    {
	        algorithm_check = priority(ready_queue, &result);
	    }
	    else if (algorithm[0] == 'R' && algorithm[1] == 'R' && algorithm[2] == '\0') //execute round robem
	    {
	        if (argc < 4) //additional parameter required
	        {
				//not valid option
	            dyn_array_destroy(ready_queue);
	            return EXIT_FAILURE;
	        }
			//convert to an integer
	        int quantum = atoi(argv[3]);
	        algorithm_check = round_robin(ready_queue, &result, quantum);
	    }
	    else if (algorithm[0] == 'S' && algorithm[1] == 'J' && algorithm[2] == 'F' && algorithm[3] == '\0')//execute shorest jor first
	    {
	        algorithm_check = shortest_job_first(ready_queue, &result);
	    }
	    else //no valid algorithm was given
		{
	        printf("Unknown algorithm.\n");
	        dyn_array_destroy(ready_queue);
	        return EXIT_FAILURE;
	    }
	
	    if (!algorithm_check)//check that all the flags passed
	    {
	        printf("Scheduling failed.\n");
	        dyn_array_destroy(ready_queue);
	        return EXIT_FAILURE;
	    }
	    //free the array
	    dyn_array_destroy(ready_queue);
	    return EXIT_SUCCESS;
}
