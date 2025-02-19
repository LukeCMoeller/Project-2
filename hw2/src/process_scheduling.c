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
/**/
//Looking online and consulting online sources most seem to have this job object
//defined for qsort comparisons functions so i have included it here
typedef struct {
	int job_id;      // The id of the job
	int burst_time;  // The burst time of the job
	int arrival_time;// The Time that it takes to arrive at this job
} job_t;

/**/
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
	//UNUSED(ready_queue);
	//UNUSED(result);
	//return false;

	//this region only functions in visual studio code
	#pragma region Going through the array and compiling the result
	//This sets up for calculating the schedule_t result result
	size_t num_jobs = dyn_array_size(ready_queue);
	int completion_time = 0;
	int current_time = 0;
	for(size_t x = 0; x < num_jobs; x++){
		//Hopefully this works
		job_t *job = (job_t *)(dyn_array_front(ready_queue)) + x;

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
	//UNUSED(ready_queue);
	//UNUSED(result);
	//return false;
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
		job_t *job = (job_t *)(dyn_array_front(ready_queue)) + x;

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
	const job_t *job1 = (const job_t *)a;
	const job_t *job2 = (const job_t *)b;
	// Sort by shortest burst time first
	return job1->burst_time - job2->burst_time; 
}

bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	//NOT being done this project
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
	//INCOMPLETE
	//create the dyn_array
	// dyn_array_t ready_queue = dyn_array_create();
	UNUSED(input_file);
	return NULL;
	//eventually return the newly created dyn_array
	// return ready_queue;
}

bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	//INCOMPLETE
	UNUSED(ready_queue);
	UNUSED(result);
	return false;
}
