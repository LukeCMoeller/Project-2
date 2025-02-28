#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#include "dyn_array.h"
#include "processing_scheduling.h"


// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

//these are logical reference based on what ScheduleResult_t *result most likely is
//defined for qsort comparisons functions so i have included it here
//Also is defined minus process_id in Section 5. Details Under: Binary Configuration File Format
//And Above Terminology and concepts
/*
typedef struct {
	int process_id;  // The id of the process
	int burst_time;  // The burst time of the process
	int arrival_time;// The Time that the process arrived to be run.
	int priority;    // The process's priority value
} process_t;
*/
/*
//This is the Project 2: Defined exit result
typedef struct {
    size_t total_clock_time;      // Total time taken to execute all processes
    int average_waiting_time;     // int storing the average waiting time of each job
    int average_turnaround_time;  // int storing the average turnaround time of each job
} ScheduleResult_t;
*/


// private function
void virtual_cpu(ProcessControlBlock_t *process_control_block) 
{
	// decrement the burst time of the pcb
	--process_control_block->remaining_burst_time;
}

int compare_remaining_time(const void *a, const void *b){
	const ProcessControlBlock_t *job1 = (const ProcessControlBlock_t *)a;
    const ProcessControlBlock_t *job2 = (const ProcessControlBlock_t *)b;

	if(job1->arrival < job2->arrival){
		return ((job1->remaining_burst_time - (job2->arrival - job1->arrival)) - job2->remaining_burst_time);
	}
	else if (job1->arrival > job2->arrival){
		return (job1->remaining_burst_time - (job2->remaining_burst_time - (job1->arrival - job2->arrival)));
	}
	else{
		return job1->remaining_burst_time - job2->remaining_burst_time;
	}
}

bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    //Hopefully this works the way I want it to
    //NOTE: needs to be qsorted by arrival time to complete the FCFS
    //UNUSED(ready_queue);
    //UNUSED(result);
    //return false;

    if(dyn_array_size(ready_queue) == 0 || !result || !ready_queue){
        return false; //Error handling
    }

	// Initialize result fields
    result->average_turnaround_time = 0;
    result->average_waiting_time = 0;

    //This sets up for calculating the schedule_t result result
    size_t num_jobs = dyn_array_size(ready_queue);
    uint32_t completion_time = 0;
    uint32_t current_time = 0;
    for(size_t x = 0; x < num_jobs; x++){
        //Hopefully this works
        // process_t *job = (process_t *)dyn_array_at(ready_queue, x);
        ProcessControlBlock_t *job = (ProcessControlBlock_t *)dyn_array_at(ready_queue, x);
		if (!job) {
            return false; // error handling
        }

        if(current_time < job->arrival){
            // CPU idle time handling
            current_time = job->arrival;
        }

        //compute the Completion Time of this job
        completion_time = current_time + job->remaining_burst_time;
        //compute Turnaround Time
        result->average_turnaround_time += completion_time - job->arrival;
        //compute Waiting Time
        result->average_waiting_time += (completion_time - job->arrival) - job->remaining_burst_time;

        // Update current_time to reflect the execution of this job
        current_time = completion_time;
    }

    //average the totals for both turnaround_time and waiting times
    if (num_jobs > 0){
		result->average_turnaround_time /= (float)num_jobs;
        result->average_waiting_time /= (float)num_jobs;
	}
    // Store total execution time
    result->total_run_time = current_time;
    return true;
}

//This should allow us to be able to properly sort the ready_queue dyn_array_t
int compare_sjf(const void *a, const void *b){
	const ProcessControlBlock_t *job1 = (const ProcessControlBlock_t *)a;
    const ProcessControlBlock_t *job2 = (const ProcessControlBlock_t *)b;

    // Sort by shortest remaining burst time first
    return job1->remaining_burst_time - job2->remaining_burst_time;
}

bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	//Error checking
	if(dyn_array_size(ready_queue) == 0 || !result || !ready_queue){
		return false; //Error handling
	}

	//Hopefully the method below will sort the dyn_array_t ready_queue
	//by shortest job first
	if(dyn_array_sort(ready_queue, compare_sjf) == false){
		return false;
	}

	//This sets up for calculating the schedule_t result result
	size_t num_jobs = dyn_array_size(ready_queue);
	uint32_t completion_time = 0;
	uint32_t current_time = 0;

	// Initialize accumulators for turnaround and waiting times
    	result->average_turnaround_time = 0;
    	result->average_waiting_time = 0;

	for(size_t x = 0; x < num_jobs; x++){
		//Hopefully this works
		ProcessControlBlock_t *job = (ProcessControlBlock_t *)dyn_array_at(ready_queue, x);
		if (!job) {
            		return false; // error handling
        	}
		if(current_time < job->arrival){
			// CPU idle time handling
			current_time = job->arrival;
		}
		
		// Compute the Completion Time of this job
              	completion_time = current_time + job->remaining_burst_time;
              	// Accumulate Turnaround Time (completion - arrival)
              	result->average_turnaround_time += (completion_time - job->arrival);
              	// Accumulate Waiting Time (turnaround - burst)
              	result->average_waiting_time += (completion_time - job->arrival - job->remaining_burst_time);

		// Update current_time to reflect the execution of this job
		current_time = completion_time;
	}

	//average the totals for both turnaround_time and waiting times
    if (num_jobs > 0){
		result->average_turnaround_time /= (float)num_jobs;
        result->average_waiting_time /= (float)num_jobs;
	}
	// Store total execution time
	result->total_run_time = current_time;
	return true;
}

//This should allow us to be able to properly sort the ready_queue dyn_array_t by priority
//this is from and based on compare_sjf (Shortest job first) method.
//Since lower priority number = higher priority 
// this will work the same way
int compare_priority(const void *a, const void *b){
	const ProcessControlBlock_t *job1 = (const ProcessControlBlock_t *)a;
    const ProcessControlBlock_t *job2 = (const ProcessControlBlock_t *)b;

    // Sort by shortest priority first
    return job1->priority - job2->priority;
}

bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	/*
	All that was done here is just take the algorithm for shortest job first and change the sorting parameters by priority
	instead of by shortest job
	*/
	//Error checking
	if(dyn_array_size(ready_queue) == 0 || !result || !ready_queue){
		return false; //Error handling
	}

	//Hopefully the method below will sort the dyn_array_t ready_queue
	//by smallest numerical priority first.
	//SINCE PRIORITY is BASED ON LOWER NUMBER = HIGHER PRIORITY
	//This should work just fine
	if(dyn_array_sort(ready_queue, compare_priority) == false){
		return false;
	}

	//This sets up for calculating the schedule_t result result
	size_t num_jobs = dyn_array_size(ready_queue);
	uint32_t completion_time = 0;
	uint32_t current_time = 0;
	for(size_t x = 0; x < num_jobs; x++){
		//Hopefully this works
		ProcessControlBlock_t *job = (ProcessControlBlock_t *)dyn_array_at(ready_queue, x);
		if (!job) {
            return false; // error handling
        }
		if(current_time < job->arrival){
			// CPU idle time handling
			current_time = job->arrival;
		}

		//compute the Completion Time of this job
		completion_time = current_time + job->remaining_burst_time;
		//compute Turnaround Time
		result->average_turnaround_time = completion_time - job->arrival;
		//compute Waiting Time
		result->average_waiting_time = result->average_turnaround_time - job->remaining_burst_time;

		// Update current_time to reflect the execution of this job
		current_time = completion_time;
	}

	//average the totals for both turnaround_time and waiting times
    if (num_jobs > 0){
		result->average_turnaround_time /= (float)num_jobs;
        result->average_waiting_time /= (float)num_jobs;
	}
	// Store total execution time
	result->total_run_time = current_time;
	return true;
}

bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum) 
{
	// Error checking: ensure valid pointers, non-empty ready_queue, and a nonzero quantum
    	if (!ready_queue || !result || dyn_array_size(ready_queue) == 0 || quantum == 0) {
        	return false;
    	}

    	size_t num_jobs = dyn_array_size(ready_queue);

    	// Initialize result accumulator
    	result->average_turnaround_time = 0;
    	result->average_waiting_time = 0;
    	result->total_run_time = 0;

    	// Arrays to track job completion and if a job is currently enqueued
    	bool *completed = calloc(num_jobs, sizeof(bool));
    	bool *in_queue  = calloc(num_jobs, sizeof(bool));
    	if (!completed || !in_queue) {
        	free(completed);
        	free(in_queue);
        	return false;
    	}

    	// Allocate a simple queue for indices (worst-case size = num_jobs)
    	int *queue = malloc(num_jobs * sizeof(int));
    	if (!queue) {
        	free(completed);
        	free(in_queue);
        	return false;
    	}
    	int front = 0, rear = 0;

    	// Initialize current_time to the earliest arrival
    	uint32_t current_time = UINT32_MAX;
    	for (size_t i = 0; i < num_jobs; i++) {
        	ProcessControlBlock_t *job = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i);
        	if (job->arrival < current_time) {
            		current_time = job->arrival;
        	}
    	}

    	// Enqueue all processes that have arrived by current_time
    	for (size_t i = 0; i < num_jobs; i++) {
        	ProcessControlBlock_t *job = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i);
        	if (job->arrival <= current_time && !in_queue[i]) {
            		queue[rear++] = i;
            		in_queue[i] = true;
        	}
    	}

    	size_t completed_count = 0;
    	// Main scheduling loop
    	while (completed_count < num_jobs) {
        	// If the queue is empty, advance current_time to the next process arrival
        	if (front == rear) {
            		uint32_t next_arrival = UINT32_MAX;
            		for (size_t i = 0; i < num_jobs; i++) {
                		if (!completed[i]) {
                    			ProcessControlBlock_t *job = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i);
                    			if (job->arrival < next_arrival) {
                        			next_arrival = job->arrival;
                    			}
                		}
            		}
            		current_time = next_arrival;
            
	    		// Enqueue any process that has now arrived
            		for (size_t i = 0; i < num_jobs; i++) {
                		if (!completed[i] && !in_queue[i]) {
                    			ProcessControlBlock_t *job = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i);
                    			if (job->arrival <= current_time) {
                        			queue[rear++] = i;
                        			in_queue[i] = true;
                    			}		
                		}
            		}
        	}

        // Dequeue the next process
        int idx = queue[front++];
        in_queue[idx] = false;
        ProcessControlBlock_t *job = (ProcessControlBlock_t *)dyn_array_at(ready_queue, idx);

        // If current_time is before the job's arrival, advance the clock
        if (current_time < job->arrival) {
            current_time = job->arrival;
        }

        // Execute the job for a time slice (quantum) or for its remaining time
        uint32_t exec_time = (job->remaining_burst_time > quantum) ? quantum : job->remaining_burst_time;
        job->remaining_burst_time -= exec_time;
        current_time += exec_time;

        // Enqueue any jobs that have arrived during this time slice
        for (size_t i = 0; i < num_jobs; i++) {
            	if (!completed[i] && !in_queue[i]) {
                	ProcessControlBlock_t *other = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i);
                	if (other->arrival <= current_time) {
                    		queue[rear++] = i;
                    		in_queue[i] = true;
                	}
            	}
        }

        // Check if the current job is complete.
       	 if (job->remaining_burst_time == 0) {
            	completed[idx] = true;
            	completed_count++;

            	// Turnaround time: current_time - arrival
            	uint32_t turnaround = current_time - job->arrival;
            	result->average_turnaround_time += turnaround;
            	
		// Waiting time: turnaround time - original burst time
            	result->average_waiting_time += (turnaround - job->burst_time);
        	} else {
            	
		// If not finished, re-enqueue the job
            	queue[rear++] = idx;
            	in_queue[idx] = true;
        	}
    	}

    	// Calculate averages
    	result->average_turnaround_time /= (float)num_jobs;
    	result->average_waiting_time /= (float)num_jobs;
    	result->total_run_time = current_time;

    	free(queue);
    	free(in_queue);
    	free(completed);
    	return true;
}

// Help with sorting by arrival time in load_process_control_blocks 
int compare_arrival_time(const void *a, const void *b) {
    const ProcessControlBlock_t *p1 = (const ProcessControlBlock_t *)a;
    const ProcessControlBlock_t *p2 = (const ProcessControlBlock_t *)b;
    if (p1->arrival < p2->arrival) return -1;
    if (p1->arrival > p2->arrival) return 1;
    return 0;
}

dyn_array_t *load_process_control_blocks(const char *input_file) 
{
	//DOUBLE CHECK FOR COMPLETION
	//UNUSED(input_file);
	//return NULL;

	if(!input_file){ //error handling for when their is nothing inserted
		return NULL;
	}

	FILE *file = fopen(input_file, "rb");
	if(!file){ //error handling for when file doesn't exist
		printf("problem with opening file %s\n", input_file);
		return NULL;
	}

	int n; //there will be 1+(3N) 32-bit integers
	if (fread(&n, sizeof(int), 1, file) != 1) {   // Need to pass the address of N, error handling to be sure we read N
		printf("problem with reading number of processes from file\n");
		fclose(file);
		return NULL;
	}

	size_t cap = n;
	//create the dyn_array
	dyn_array_t *ready_queue = dyn_array_create(cap, sizeof(ProcessControlBlock_t), NULL);
	if(!ready_queue){
		printf("problem with creating dyn_array\n");
		fclose(file);
		return NULL; //error handling incase dyn_array_create() has failed
	}

	//process_t process;
	ProcessControlBlock_t process;

	//We now cycle through the file and insert information
	for(uint32_t x = 0; x < (uint32_t)n; x++){
		//process.process_id = x;
		if (
			fread(&process.remaining_burst_time, sizeof(uint32_t), 1, file) != 1 ||  //grab values and put them into a process_t struct
			fread(&process.priority, sizeof(uint32_t), 1, file) != 1 ||     //grab values and put them into a process_t struct
			fread(&process.arrival, sizeof(uint32_t), 1, file) != 1) { //grab values and put them into a process_t struct
			dyn_array_destroy(ready_queue); //prevent mem leak
			printf("problem with reading remaining burst time\n");
			fclose(file);
			return NULL;
		}
		
		// Set the original burst time to the read burst time
		process.burst_time = process.remaining_burst_time;

		process.started = false; // process hasn't started yet
		
		if (!dyn_array_push_back(ready_queue, &process)) { // Push process to queue
			printf("Process %d: priority=%u\n", x, process.priority);
			dyn_array_destroy(ready_queue);
            		fclose(file);
            		return NULL;
        	}
	}

	qsort((void *)dyn_array_export(ready_queue), 
      		dyn_array_size(ready_queue), 
      		sizeof(ProcessControlBlock_t), 
      		compare_arrival_time);

	
	fclose(file); //close file
	return ready_queue; //eventually return the newly created dyn_array
}

bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	/*
	This one wants to sort all by shortest burst time to start off with and
	EVERY time it completes a process it wants to resort by shortest burst time

	*/
	//Error checking
	if(dyn_array_size(ready_queue) == 0 || !result || !ready_queue){
		return false; //Error handling
	}

	//Hopefully the method below will sort the dyn_array_t ready_queue
	if(dyn_array_sort(ready_queue, compare_remaining_time) == false){
		return false;
	}

	//This sets up for calculating the schedule_t result result
	size_t num_jobs = dyn_array_size(ready_queue);
	uint32_t completion_time = 0;
	uint32_t current_time = 0;
	size_t x = 0;
	while(x != num_jobs){
		//Hopefully this works
		ProcessControlBlock_t *job = (ProcessControlBlock_t *)dyn_array_at(ready_queue, 0);
		if (!job) {
            return false; // error handling
        }
		if(current_time < job->arrival){
			// CPU idle time handling
			current_time = job->arrival;
		}

		//compute the Completion Time of this job
		completion_time = current_time + job->remaining_burst_time;
		//compute Turnaround Time
		result->average_turnaround_time = completion_time - job->arrival;
		//compute Waiting Time
		result->average_waiting_time = result->average_turnaround_time - job->remaining_burst_time;

		// Update current_time to reflect the execution of this job
		current_time = completion_time;
		//remove first element of queue
		if(dyn_array_pop_front(ready_queue) == false){
			break; //We have no elements in the list and we should leave
		}

		//WE can sort here, but it more than likly won't have any changes or differences
		if (dyn_array_sort(ready_queue, compare_remaining_time) == false) {
			return false;
		}
		x++;
	}

	//average the totals for both turnaround_time and waiting times
    if (num_jobs > 0){
		result->average_turnaround_time /= (float)num_jobs;
        result->average_waiting_time /= (float)num_jobs;
	}
	// Store total execution time
	result->total_run_time = current_time;
	return true;
}

/*
This should in theory sort by smallest remaining time.
This is done by first comparing arrival time.
Then based on who got their first, we then compare if it's remaining burst time
EX.
Arrival = a, remaing burst time = b
# | a | b
1 | 0 | 10
2 | 15| 7
3 | 7 | 4
4 | 5 | 7
5 | 30| 10

would sort into order
1 3 4 2 5 
*/

