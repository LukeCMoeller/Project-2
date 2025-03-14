#include <fcntl.h>
#include <stdio.h>
#include "gtest/gtest.h"
#include <pthread.h>
#include "../include/processing_scheduling.h"
#include <dyn_array.h>

// Using a C library requires extern "C" to prevent function managling
extern "C" 
{
#include <dyn_array.h>
#include <processing_scheduling.h>
}

#define NUM_PCB 30
#define QUANTUM 5 // Used for Robin Round for process as the run time limit


//had to add the structure here because for some reason it wouldnt recognize what it was otherwise
struct dyn_array 
{
	// DYN_FLAGS flags;
	size_t capacity;
	size_t size;
	const size_t data_size;
	void *array;
	void (*destructor)(void *);
};
//testing for dyn_array_create
TEST(dyn_array_create, ValidArray){
	dyn_array_t *test_array = dyn_array_create(10, sizeof(int), NULL);
	EXPECT_NE(test_array, nullptr); //check that array exists
	int size = test_array->size;
	EXPECT_EQ(size, 0);
	free(test_array);
}
//testing for dyn_array_create
TEST(dyn_array_create, InvalidCreateArray){
	dyn_array_t *test_array = dyn_array_create(-1, sizeof(int), NULL);
	EXPECT_EQ(test_array, nullptr); //check that array dostn exist
	
	test_array = dyn_array_create(10, 0, NULL);
	EXPECT_EQ(test_array, nullptr); //check that dostn exist
}

//testing for dyn_array_import
TEST(DynArrayImportTest, ImportArray){
	int test_nums[] = {2, 3, 6, 8, 9, 11};
	size_t count = 6;
	size_t int_size = sizeof(int);
	dyn_array_t *test_array = dyn_array_import(test_nums, count, int_size, NULL);
	ASSERT_NE(test_array, nullptr); //check import works
	EXPECT_EQ(test_array->size, count); //check size is equial
	int *imported_data = (int *)test_array->array;
	for(size_t i = 0; i < count; i++){
		EXPECT_EQ(imported_data[i], test_nums[i]); //check import matches original data
	}
	free(test_array);
}
//Testing for dyn_array_import
TEST(DynArrayImportTest, InvalidImportArray) {
    int *test_nums = nullptr;
    size_t count = 6;
    size_t int_size = sizeof(int);
    dyn_array_t *test_array = dyn_array_import(test_nums, count, int_size, NULL);
    EXPECT_EQ(test_array, nullptr); //Check import fails

    int test_nums_invalid[] = {2, 3, 6, 8, 9, 11};
    count = 0;
    test_array = dyn_array_import(test_nums_invalid, count, int_size, NULL);
    EXPECT_EQ(test_array, nullptr); //Check that the import fails

    count = 6;
    int_size = 0;
    test_array = dyn_array_import(test_nums_invalid, count, int_size, NULL);
    EXPECT_EQ(test_array, nullptr); //Check that the import fails
}

//testing for dyn_array_export
TEST(DynArrayExportTest, ExportArray){
	int test_nums[] = {2, 3, 6, 8, 9, 11};
	size_t count = 6;
	size_t int_size = sizeof(int);
	dyn_array_t *test_array = dyn_array_import(test_nums, count, int_size, NULL);
	ASSERT_NE(test_array, nullptr); //check import works
	const void *export_array = dyn_array_export(test_array);
    ASSERT_NE(export_array, nullptr); //check export works
	int *exported_data = (int *)test_array->array;
	for(size_t i = 0; i < count; i++){
		EXPECT_EQ(exported_data[i], test_nums[i]); //check export matches original data
	}
	free(test_array);
}
//testing for dyn_array_export
TEST(DynArrayExportTest, InvalidExportArray) {
    dyn_array_t *test_array = nullptr;
    const void *export_array = dyn_array_export(test_array);
    EXPECT_EQ(export_array, nullptr); //check that export fails
}

//testing for dny_array_destroy
TEST(DynArrayDestroyTest, DestroyArray){
	dyn_array_t *test_array = dyn_array_create(10, sizeof(int), NULL);
	ASSERT_NE(test_array, nullptr); //check array was created
	dyn_array_destroy(test_array);//destory array
	test_array = NULL; //set pointer to null

	EXPECT_EQ(test_array, nullptr);//confirm pointer is null
}
//testing for dyn_array_destroy
TEST(DynArrayDestroyTest, InvalidDestroyArray) {
    dyn_array_t *test_array = nullptr;
    dyn_array_destroy(test_array);
    EXPECT_EQ(test_array, nullptr); //confirm pointer null
}


//testing dyn_array_front
TEST(DynArrayFrontTest, CorrectFront){
	int test_nums[] = {2, 3, 6, 8, 9, 11};
	size_t count = 6;
	size_t int_size = sizeof(int);
	dyn_array_t *test_array = dyn_array_import(test_nums, count, int_size, NULL);
	ASSERT_NE(test_array, nullptr); //check import works
	int* topNum = (int*)dyn_array_front(test_array);
    EXPECT_EQ(*topNum, test_nums[0]);//check first num is the same
	dyn_array_destroy(test_array);
}
//testing dyn_array_front
TEST(DynArrayFrontTest, InvalidArray){
	dyn_array_t *test_array = dyn_array_create(10, sizeof(int), NULL);
	EXPECT_NE(test_array, nullptr); //check that array exists
	int* topNum = (int*)dyn_array_front(test_array);
    EXPECT_EQ(topNum, nullptr);//check that NULL is returned
	dyn_array_destroy(test_array);
}

//testing dyn_array_push_front
TEST(DynArrayPushFrontTest, CorrectPushTest){
	int test_nums[] = {2, 3, 6, 8, 9, 11};
	size_t count = 6;
	size_t int_size = sizeof(int);
	int num_to_add = 1;
	dyn_array_t *test_array = dyn_array_import(test_nums, count, int_size, NULL);
	ASSERT_NE(test_array, nullptr); //check import works
	
	bool check = dyn_array_push_front(test_array, &num_to_add);
	EXPECT_EQ(check, true); //confirm that it passed
	EXPECT_EQ(test_array->size, count + 1); //check size is 1 greater than original size
	
	int* topNum = (int*)dyn_array_front(test_array);
    EXPECT_EQ(*topNum, num_to_add); //check first num is the new added number

	dyn_array_destroy(test_array);
}

//testing dyn_array_push_front
TEST(DynArrayPushFrontTest, IncorrectPushTest){
	int num_to_add = 1;
 	dyn_array_t *wrong_array = nullptr;
 	bool check = dyn_array_push_front(wrong_array, &num_to_add);
 	EXPECT_EQ(check, false); //cofirm that it failed
}

//testing dyn_array_pop_front
TEST(DynArrayPopFrontTest, SuccessfulPop){
	int test_nums[] = {2, 3, 6, 8, 9, 11};
	size_t count = 6;
	size_t int_size = sizeof(int);
	dyn_array_t *test_array = dyn_array_import(test_nums, count, int_size, NULL);
	ASSERT_NE(test_array, nullptr); //check import works
	bool check = dyn_array_pop_front(test_array);
	EXPECT_EQ(check, true); //confirm that it passed
	EXPECT_EQ(test_array->size, count - 1); //check size is 1 less than original size
	
	int* topNum = (int*)dyn_array_front(test_array);
    EXPECT_EQ(*topNum, test_nums[1]); //check first num is the now the second number of the original array

	dyn_array_destroy(test_array);
}
//testing dyn_array_pop_front
TEST(DynArrayPophFrontTest, InvalidPop){
	dyn_array_t *test_array = dyn_array_create(10, sizeof(int), NULL);
	bool check = dyn_array_pop_front(test_array);
	EXPECT_EQ(check, false); //confirm that it failed
 	
 	dyn_array_t *wrong_array = nullptr;
 	check = dyn_array_pop_front(wrong_array);
 	EXPECT_EQ(check, false); //confirm that it failed
}

//testing dyn_array_extract_front
TEST(DynArrayExtractFrontTest, SuccessfulExtract) {
    int test_nums[] = {2, 3, 6, 8, 9, 11};
    size_t count = 6;
    size_t int_size = sizeof(int);
    dyn_array_t *test_array = dyn_array_import(test_nums, count, int_size, NULL);
    ASSERT_NE(test_array, nullptr); //check import works

    int extracted;
    bool check = dyn_array_extract_front(test_array, &extracted);
    EXPECT_EQ(check, true); //confirm extraction
    EXPECT_EQ(extracted, test_nums[0]); //check the value is the first number of our array

    EXPECT_EQ(test_array->size, count - 1); //size should be 1 less than original

    int* topNum = (int*)dyn_array_front(test_array);
    EXPECT_EQ(*topNum, test_nums[1]); //check first num is now the second one

    dyn_array_destroy(test_array);
}
//testing dyn_array_extract_front
TEST(DynArrayExtractFrontTest, EmptyArrayExtract) {
    dyn_array_t *test_array = dyn_array_create(10, sizeof(int), NULL);
    ASSERT_NE(test_array, nullptr); //check array creation

    int extracted;
    bool check = dyn_array_extract_front(test_array, &extracted);
    EXPECT_EQ(check, false); //confirm the extraction failed since the array is empty

    dyn_array_destroy(test_array);
}

//testing dyn_array_back
TEST(DynArrayBackTest, ValidBack) {
    int test_nums[] = {2, 3, 6, 8, 9, 11};
    size_t count = 6;
    size_t int_size = sizeof(int);
    dyn_array_t *test_array = dyn_array_import(test_nums, count, int_size, NULL);
    ASSERT_NE(test_array, nullptr); //check import works

    int *last_number = (int *)dyn_array_back(test_array);
    ASSERT_NE(last_number, nullptr); //ensure pointer exists
    EXPECT_EQ(*last_number, test_nums[count - 1]); //check the last num is correct

    dyn_array_destroy(test_array);
}
//testing dyn_array_back
TEST(DynArrayBackTest, EmptyArrayBack) {
    dyn_array_t *test_array = dyn_array_create(10, sizeof(int), NULL);
    ASSERT_NE(test_array, nullptr); //check array creation

    int* last_num = (int *)dyn_array_back(test_array);
    EXPECT_EQ(last_num, nullptr); //confirm NULL for an empty array

    dyn_array_destroy(test_array);
}

//testing dyn_array_push_back
TEST(DynArrayPushBackTest, ValidPush) {
    int test_nums[] = {2, 3, 6, 8, 9, 11};
    size_t count = 6;
    size_t int_size = sizeof(int);
    dyn_array_t *test_array = dyn_array_import(test_nums, count, int_size, NULL);
    ASSERT_NE(test_array, nullptr); //check import works

    int num_to_add = 12;
    bool check = dyn_array_push_back(test_array, &num_to_add);
    EXPECT_EQ(check, true); //confirm the push was successful
    EXPECT_EQ(test_array->size, count + 1); //check size is greater than 1 of origianl

    int *last_num = (int *)dyn_array_back(test_array);
    EXPECT_EQ(*last_num, num_to_add); //check that the new element is at end of array

    dyn_array_destroy(test_array);
}
//testing dyn_array_push_back with NULL array
TEST(DynArrayPushBackTest, NullArrayPush) {
    dyn_array_t *empty_array = nullptr;
    int num_to_add = 5;
    bool check = dyn_array_push_back(empty_array, &num_to_add);
    EXPECT_EQ(check, false); //confirm failed with null array
}

//testing dyn_array_pop_back
TEST(DynArrayPopBackTest, ValidPop) {
    int test_nums[] = {2, 3, 6, 8, 9, 11};
    size_t count = 6;
    size_t int_size = sizeof(int);
    dyn_array_t *test_array = dyn_array_import(test_nums, count, int_size, NULL);
    ASSERT_NE(test_array, nullptr); //check import works

    bool check = dyn_array_pop_back(test_array);
    EXPECT_EQ(check, true); //confirm pop was successful
    EXPECT_EQ(test_array->size, count - 1); //check size of array decreased by 1

    int *last_num = (int *)dyn_array_back(test_array);
    EXPECT_NE(last_num, nullptr); //make sure the last element is now null

    dyn_array_destroy(test_array);
}
//testing dyn_array_pop_back
TEST(DynArrayPopBackTest, EmptyArrayPop) {
    dyn_array_t *empty_array = dyn_array_create(0, sizeof(int), NULL);
    ASSERT_NE(empty_array, nullptr); //make sure the empty array is created

    bool check = dyn_array_pop_back(empty_array);
    EXPECT_EQ(check, false); //confirm pop fails with empty array
    dyn_array_destroy(empty_array);
}

/*
unsigned int score;
unsigned int total;

class GradeEnvironment : public testing::Environment
{
	public:
		virtual void SetUp()
		{
			score = 0;
			total = 210;
		}

		virtual void TearDown()
		{
			::testing::Test::RecordProperty("points_given", score);
			::testing::Test::RecordProperty("points_total", total);
			std::cout << "SCORE: " << score << '/' << total << std::endl;
		}
};
*/

//these tests are for process_scheduling.c for the FCFS implementation
//
TEST(first_come_first_serve, Success){
	ProcessControlBlock_t test_nums[] = {	// Use process_t instead of normal int array
        	{6, 6, 1, 0, false},
        	{8, 8, 1, 2, false}
   	};
    	size_t count = sizeof(test_nums) / sizeof(ProcessControlBlock_t);
    	size_t struct_size = sizeof(ProcessControlBlock_t);
    	
	dyn_array_t *test_array = dyn_array_create(count, struct_size, NULL);
	ASSERT_NE(test_array, nullptr);

	for (size_t i = 0; i < count; i++) {	//Push test data into dynamic array
        	dyn_array_push_back(test_array, &test_nums[i]);
    	}
	
	ScheduleResult_t result = {0, 0, 0}; //set up result object that will be passed back
	
	result.total_run_time = 0; //set up default values
	result.average_waiting_time = 0;     //set up default values
	result.average_turnaround_time = 0;  //set up default values
	
	bool check = first_come_first_serve(test_array, &result); //Run the test result
	EXPECT_EQ(check, true); //if passes will be true
	
	EXPECT_GT(result.average_waiting_time, 0);     //confirm ScheduleResult_t result changed accordingly
	EXPECT_GT(result.average_turnaround_time, 0);  //confirm ScheduleResult_t result changed accordingly
	EXPECT_GT(result.total_run_time, (size_t)0); //confirm ScheduleResult_t result changed accordingly
	
	dyn_array_destroy(test_array);
}

TEST(first_come_first_serve, Fails){
	//int test_nums[] = {2, 3, 6, 8, 9, 11};
    	size_t count = 6;
    	//size_t int_size = sizeof(int);
    	
	dyn_array_t *test_array = dyn_array_create(count, sizeof(ProcessControlBlock_t), NULL);
	ASSERT_NE(test_array, nullptr);

	dyn_array_clear(test_array);	// Clear the array without modifying the size directly

	ScheduleResult_t result; //set up result object that will be passed back
	result.total_run_time = 0; //set up default values
	result.average_waiting_time = 0;     //set up default values
	result.average_turnaround_time = 0;  //set up default values
	
	bool check = first_come_first_serve(test_array, &result); //Run the test result
	
	EXPECT_EQ(check, false); //if passes will be true
	
	dyn_array_destroy(test_array);
}

TEST(shortest_remaining_time_first, Success) {
    // Define the test processes
    ProcessControlBlock_t test_process[] = {
        {6, 6, 1, 0, false}, 
        {8, 8, 1, 2, false},
        {3, 3, 1, 1, false} 
    };

    //get the number of processes and the size of the structure
    size_t count = sizeof(test_process) / sizeof(ProcessControlBlock_t);
    size_t struct_size = sizeof(ProcessControlBlock_t);

    //create a dynamic array to hold the processes
    dyn_array_t *test_array = dyn_array_create(count, struct_size, NULL);
    ASSERT_NE(test_array, nullptr); //ensure the array is created

    //push each process onto the array
    for (size_t i = 0; i < count; i++) {
        dyn_array_push_back(test_array, &test_process[i]);
    }


    ScheduleResult_t result = {0, 0, 0};  //nitialize results

    //run shortest_remaining_time_first
    bool check = shortest_remaining_time_first(test_array, &result);
	
	EXPECT_EQ(check, true); //ensure function runs successfully
    EXPECT_GT(result.total_run_time, (uint32_t)0);
    EXPECT_GE(result.average_waiting_time, (float)0);
    EXPECT_GE(result.average_turnaround_time, (float)0);

    dyn_array_destroy(test_array);
}

//tests for shotest_job_first
TEST(shortest_remaining_time_first, Nullfail){
	dyn_array_t *test_array = nullptr;
	ScheduleResult_t *test_result = nullptr;

	bool check = shortest_remaining_time_first(test_array, test_result); 
	EXPECT_EQ(check, false);
}
//These Tests are for process_scheduling.c for the load_process_control_blocks (PCB)

TEST(load_process_control_blocks, Success){	
	const char *test_input_file = "test_pcb.bin";

    	// Create a test binary file with predefined PCB data
    	FILE *file = fopen(test_input_file, "wb");
    	ASSERT_NE(file, nullptr);

    	uint32_t test_data[] = {
        	2,  // Number of processes
        	5, 1, 0,  // Process 1: burst time, priority, arrival time
        	3, 2, 2   // Process 2: burst time, priority, arrival time
    	};

    	// Write test data to the file
    	size_t elements_written = fwrite(test_data, sizeof(uint32_t), sizeof(test_data) / sizeof(uint32_t), file);
    	ASSERT_EQ(elements_written, sizeof(test_data) / sizeof(uint32_t));
    	fclose(file);

    	// Load the process control blocks from the binary file
    	dyn_array_t *loaded_list = load_process_control_blocks(test_input_file);
    	ASSERT_NE(loaded_list, nullptr);
    	EXPECT_EQ(dyn_array_size(loaded_list),(size_t)2);

    	// Validate the first process
    	ProcessControlBlock_t *pcb1 = (ProcessControlBlock_t *)dyn_array_at(loaded_list, 0);
    	ASSERT_NE(pcb1, nullptr);
    	EXPECT_EQ(pcb1->remaining_burst_time,(uint32_t)5);
    	EXPECT_EQ(pcb1->priority, (uint32_t)1);
    	EXPECT_EQ(pcb1->arrival, (uint32_t)0);

    	// Validate the second process
    	ProcessControlBlock_t *pcb2 = (ProcessControlBlock_t *)dyn_array_at(loaded_list, 1);
    	ASSERT_NE(pcb2, nullptr);
    	EXPECT_EQ(pcb2->remaining_burst_time,(uint32_t)3);
    	EXPECT_EQ(pcb2->priority, (uint32_t)2);
    	EXPECT_EQ(pcb2->arrival, (uint32_t)2);

    	// Cleanup
    	dyn_array_destroy(loaded_list);
    	remove(test_input_file);
}

TEST(load_process_control_blocks, Fail){
    	const char *test_input_file = nullptr;  // Invalid file path (nullptr simulates a missing file)
    
    	dyn_array_t *loaded_list = load_process_control_blocks(test_input_file);
    
    	EXPECT_EQ(loaded_list, nullptr);  // Expect failure (should return nullptr)
}
//tests for shotest_job_first
TEST(shortest_job_first, Nullfail){
    	dyn_array_t *test_array = nullptr;
    	ScheduleResult_t *test_result = nullptr;

    	bool check = shortest_job_first(test_array, test_result); 
    	EXPECT_EQ(check, false);
}

TEST(shortest_job_first, ShortJobSucess){
    	//define the test processes
    	ProcessControlBlock_t test_process[] = { 
    	    {5, 5, 1, 0, false},  
    		{3, 3, 1, 1, false}, 
    	    {8, 8, 1, 2, false}   
    	};
    
    	//get the number of processes and the size of the structure
    	size_t count = sizeof(test_process) / sizeof(ProcessControlBlock_t);
    	size_t struct_size = sizeof(ProcessControlBlock_t);

    	//create a dynamic array to hold the processes
    	dyn_array_t *test_array = dyn_array_create(count, struct_size, NULL);
   		ASSERT_NE(test_array, nullptr); //make sure array is created

    	//push each onto the array
    	for (size_t i = 0; i < count; i++) {
    	    dyn_array_push_back(test_array, &test_process[i]);
    	}

    	//initialize results
    	ScheduleResult_t result = {0, 0, 0};

    	//run
    	bool check = shortest_job_first(test_array, &result);
    	EXPECT_EQ(check, true);//ensure sucess

    	EXPECT_GT(result.total_run_time, (uint32_t)0);  //total run time > 0
    	EXPECT_GT(result.average_waiting_time, (float)0);  //average waiting time > 0
    	EXPECT_GT(result.average_turnaround_time, (float)0);  //average turnaround time > 0

    	dyn_array_destroy(test_array);
}
//tests for round_robin
TEST(round_robin, Nullfail){
		dyn_array_t *test_array = nullptr;
		ScheduleResult_t *test_result = nullptr;

		bool check = round_robin(test_array, test_result, QUANTUM); 
		EXPECT_EQ(check, false);
}
TEST(round_robin, Success){
    	//define test processes
    	ProcessControlBlock_t test_process[NUM_PCB] = { 
    	    {6, 6, 1, 0, false},  
    	    {8, 8, 1, 2, false}  
    	};
    
    	//get the number of processes and the size of structure
    	size_t count = sizeof(test_process) / sizeof(ProcessControlBlock_t);
    	size_t struct_size = sizeof(ProcessControlBlock_t);

    //create a dynamic array to hold the processes
    dyn_array_t *test_array = dyn_array_create(count, struct_size, NULL);
    ASSERT_NE(test_array, nullptr);  //ensure the array is successfully created

    //push each test
    for (size_t i = 0; i < count; i++) {
        dyn_array_push_back(test_array, &test_process[i]);
    }

    ScheduleResult_t result = {0, 0, 0}; //make results 
    size_t quantum = QUANTUM;  //set quantum value

    //run the round_robin
    bool check = round_robin(test_array, &result, quantum);
    EXPECT_EQ(check, true);  //expect true

    EXPECT_GT(result.total_run_time, (uint32_t)0);  // Total run time > 0
    EXPECT_GT(result.average_waiting_time, (float)0);  // Average waiting time > 0
    EXPECT_GT(result.average_turnaround_time, (float)0);  // Average turnaround > 0

    dyn_array_destroy(test_array);
}

//tests for priority
TEST(priority, Nullfail){
	dyn_array_t *test_array = nullptr;
	ScheduleResult_t *test_result = nullptr;

	bool check = priority(test_array, test_result); 
	EXPECT_EQ(check, false);
}
TEST(priority, SuccessTest){
    //define an array
    ProcessControlBlock_t test_process[] = { 
        {6, 6, 1, 0, false},  
        {8, 8, 2, 2, false},  
        {4, 4, 1, 3, false} 
    };

    //get the number of processes and the size of structure
    size_t count = sizeof(test_process) / sizeof(ProcessControlBlock_t);
    size_t struct_size = sizeof(ProcessControlBlock_t);

    //create a dynamic array to hold the processes
    dyn_array_t *test_array = dyn_array_create(count, struct_size, NULL);
    ASSERT_NE(test_array, nullptr);  //ensure the array is successfully created

    //push each test
    for (size_t i = 0; i < count; i++) {
        dyn_array_push_back(test_array, &test_process[i]);
    }

    ScheduleResult_t result = {0, 0, 0};  //initialize result

    //run priority
    bool check = priority(test_array, &result);
    EXPECT_EQ(check, true);  //expect true 

    EXPECT_GT(result.total_run_time, (uint32_t)0);  //total run time > 0
    EXPECT_GT(result.average_waiting_time, (float)0);  //average waiting time > 0
    EXPECT_GT(result.average_turnaround_time, (float)0);  //average turnaround time > 0

    dyn_array_destroy(test_array);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	// ::testing::AddGlobalTestEnvironment(new GradeEnvironment);
	return RUN_ALL_TESTS();
}
