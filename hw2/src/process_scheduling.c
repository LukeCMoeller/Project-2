#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dyn_array.h"
#include "processing_scheduling.h"


// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

//these are logical reference based on what ScheduleResult_t *result most likely is
//defined for qsort comparisons functions so i have included it here
//Also is defined minus process_id in Section 5. Details Under: Binary Configuration File Format
//And Above Terminology and concepts
typedef struct {
	int process_id;  // The id of the process
	int burst_time;  // The burst time of the process
	int arrival_time;// The Time that the process arrived to be run.
	int priority;    // The process's priority value
} process_t;

//This is the Project 2: Defined exit result
typedef struct {
    size_t total_clock_time;      // Total time taken to execute all processes
    int average_waiting_time;     // int storing the average waiting time of each job
    int average_turnaround_time;  // int storing the average turnaround time of each job
} ScheduleResult_t;



// private function
void virtual_cpu(ProcessControlBlock_t *process_control_block) 
{
	// decrement the burst time of the pcb
	--process_control_block->remaining_burst_time;
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
	//this region only functions in visual studio code
	#pragma region Going through the array and compiling the result
	//This sets up for calculating the schedule_t result result
	size_t num_jobs = dyn_array_size(ready_queue);
	int completion_time = 0;
	int current_time = 0;
	for(size_t x = 0; x < num_jobs; x++){
		//Hopefully this works
		process_t *job = (process_t *)(dyn_array_front(ready_queue)) + x;

		if(current_time < job->arrival_time){
			// CPU idle time handling
			current_time = job->arrival_time;
		}

		//compute the Completion Time of this job
		completion_time = current_time + job->burst_time;
		//compute Turnaround Time
		result->average_turnaround_time = completion_time - job->arrival_time;
		//compute Waiting Time
		result->average_waiting_time = result->average_turnaround_time - job->burst_time;

		// Update current_time to reflect the execution of this job
		current_time = completion_time;
	}

	//average the totals for both turnaround_time and waiting times
	result->average_turnaround_time = result->average_turnaround_time / num_jobs;
	result->average_waiting_time = result->average_waiting_time / num_jobs;
	// Store total execution time
	result->total_clock_time = current_time;
	#pragma endregion
	return true;
}

bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	//Hopefully this works the way I want it to
	/*
	UNUSED(ready_queue);
	UNUSED(result);
	return false;
	*/
	
	//Error checking
	if(dyn_array_size(ready_queue) == 0 || !result || !ready_queue){
		return false; //Error handling
	}

	//Hopefully the method below will sort the dyn_array_t ready_queue
	//by shortest job first
	if(dyn_array_sort(ready_queue, compare_sjf) == false){
		return false;
	}

	//this region only functions in visual studio code
	#pragma region Going through the array and compiling the result
	//This sets up for calculating the schedule_t result result
	size_t num_jobs = dyn_array_size(ready_queue);
	int completion_time = 0;
	int current_time = 0;
	for(size_t x = 0; x < num_jobs; x++){
		//Hopefully this works
		process_t *job = (process_t *)(dyn_array_front(ready_queue)) + x;

		if(current_time < job->arrival_time){
			// CPU idle time handling
			current_time = job->arrival_time;
		}

		//compute the Completion Time of this job
		completion_time = current_time + job->burst_time;
		//compute Turnaround Time
		result->average_turnaround_time = completion_time - job->arrival_time;
		//compute Waiting Time
		result->average_waiting_time = result->average_turnaround_time - job->burst_time;

		// Update current_time to reflect the execution of this job
		current_time = completion_time;
	}

	//average the totals for both turnaround_time and waiting times
	result->average_turnaround_time = result->average_turnaround_time / num_jobs;
	result->average_waiting_time = result->average_waiting_time / num_jobs;
	// Store total execution time
	result->total_clock_time = current_time;
	#pragma endregion
	return true;
}

//This should allow us to be able to properly sort the ready_queue dyn_array_t
int compare_sjf(const void *a, const void *b){
	const process_t *job1 = (const process_t *)a;
	const process_t *job2 = (const process_t *)b;
	// Sort by shortest burst time first
	return job1->burst_time - job2->burst_time; 
}

bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	UNUSED(ready_queue);
	UNUSED(result);
	return false;
}

bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum) 
{
	//INCOMPLETE
	UNUSED(ready_queue);
	UNUSED(result);
	UNUSED(quantum);
	return false;
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
		return NULL;
	}

	int n; //there will be 1+(3N) 32-bit integers
	fread(n, sizeof(int), 1, file); // The First number is always N
	size_t cap = (n/3);
	//create the dyn_array
	dyn_array_t *ready_queue = dyn_array_create(cap, sizeof(process_t), 0);
	if(!ready_queue){
		return NULL; //error handling incase dyn_array_create() has failed
	}

	process_t process;
	//We now cycle through the file and insert information
	for(int x = 0; x < n; x++){
		process.process_id = x;
		fread(process.burst_time, sizeof(int), 1, file);   //grab values and put them into a process_t struct
		fread(process.priority, sizeof(int), 1, file);     //grab values and put them into a process_t struct
		fread(process.arrival_time, sizeof(int), 1, file); //grab values and put them into a process_t struct
		
		bool success = dyn_array_push_back(ready_queue, &process); //Queues are FIFO so we always push to the back of the void *array that dyn_array_t is apart of
		if(success == false){
			return NULL; //error handling incase dyn_array_push_back has failed
		}
	}

	fclose(file); //close file
	return ready_queue; //eventually return the newly created dyn_array
}

bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	//INCOMPLETE
	UNUSED(ready_queue);
	UNUSED(result);
	return false;
}
