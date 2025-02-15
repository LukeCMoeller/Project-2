#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include "gtest/gtest.h"
#include "../include/processing_scheduling.h"

// Using a C library requires extern "C" to prevent function mangling
extern "C" 
{
#include <dyn_array.h>
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


int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	// ::testing::AddGlobalTestEnvironment(new GradeEnvironment);
	return RUN_ALL_TESTS();
}
