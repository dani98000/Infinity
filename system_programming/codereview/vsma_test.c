#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vsma.h"

#define TEST_EQUAL(res, expected) \
printf("\ttest %lu: expected: %d, got: %d\n",\
++test_no, expected, res); result *= (expected == res); \

#define RUN_TEST(test)\
if(!test())\
{\
printf("\033[1;31m");\
printf(#test " - failed!\n");\
printf("\033[0m");\
}\
else\
{\
printf("\033[1;32m");\
printf(#test " - ok!\n");\
printf("\033[0m");\
}

int Test_VSMAInit();
int Test_VSMAAlloc();
int Test_VSMACount();
int Test_VSMAFindLargestFree();
int Test_VSMAFree();
int Test_Flow1();

int DTest_VSMAInit();
int DTest_VSMAAlloc();
int DTest_VSMACount();
int DTest_VSMAFindLargestFree();
int DTest_VSMAFree();
int DTest_Flow1();

int main()
{
	#ifndef NDEBUG
	RUN_TEST(DTest_VSMAInit);
	RUN_TEST(DTest_VSMAAlloc);
	RUN_TEST(DTest_VSMACount);
	RUN_TEST(DTest_VSMAFindLargestFree);	
	RUN_TEST(DTest_VSMAFree);
	RUN_TEST(DTest_Flow1);	
	#else
	RUN_TEST(Test_VSMAInit);
	RUN_TEST(Test_VSMAAlloc);
	RUN_TEST(Test_VSMACount);
	RUN_TEST(Test_VSMAFindLargestFree);	
	RUN_TEST(Test_VSMAFree);
	RUN_TEST(Test_Flow1);	
	#endif
	
	return 0;
}

int Test_VSMAInit()
{
	int result = 1;
	size_t test_no = 0;
	int res = 0;
	vsma_t *vsma = (vsma_t *)malloc(500 * sizeof(char));
	memset(vsma,5,464);
	vsma = VSMAInit(vsma, 500);
		
	/* test 1 */
	res = *(size_t *)((char *)vsma);
	TEST_EQUAL(res, 480);
		
	/* test 2 */
	res = *(size_t *)((char *)vsma + 480);
	TEST_EQUAL(res, 0);	
	
	return result;
}

int Test_VSMAAlloc()
{
	int result = 1;
	size_t test_no = 0;
	int res = 0;
	vsma_t *vsma = (vsma_t *)malloc(500 * sizeof(char));
	vsma_t *temp = NULL; 
	memset(vsma, 5, 464);
	vsma = VSMAInit(vsma, 500);

	temp = VSMAAlloc(vsma, 199);	
	
	/* test 1 */
	res = *(size_t *)((char *)temp - 8);
	TEST_EQUAL(res, -200);
	
	/* test 2 */	
	res = *(size_t *)((char *)temp + 200);
	TEST_EQUAL(res, 272);
	
	/* test 3 */	
	res = *(size_t *)((char *)vsma + 208);
	TEST_EQUAL(res, 272);
	
	/* test 4 */
	res = *(size_t *)((char *)vsma );
	TEST_EQUAL(res, -200);
	
	temp = VSMAAlloc(vsma, 100);	
	
	/* test 5 */
	res = *(size_t *)((char *)temp - 8);
	TEST_EQUAL(res, -104);
	
	/* test 6 */
	res = *(size_t *)((char *)vsma);
	TEST_EQUAL(res, -200);
	
	/* test 7 */
	res = *(size_t *)((char *)temp + 104);
	TEST_EQUAL(res, 160);	
	
	/* test 8 */
	res = *(size_t *)((char *)vsma + 320);
	TEST_EQUAL(res, 160);
	
	return result;
}

int Test_VSMACount()
{
	int result = 1;
	size_t test_no = 0;
	int res = 0;
	vsma_t *vsma = (vsma_t *)malloc(500 * sizeof(char));
	vsma_t *alloc1 = NULL;
	vsma_t *alloc2 = NULL;	 
	memset(vsma, 5, 464);
	vsma = VSMAInit(vsma, 500);	
	
	/* test 1 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 480);
	
	alloc1 = VSMAAlloc(vsma, 199);
	
	/* test 2 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 272);
	
	alloc2 = VSMAAlloc(vsma, 100);
	
	/* test 3 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 160);
	
	VSMAFree(alloc1);
	
	/* test 4 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 360);
	
	VSMAFree(alloc2);
	
	/* test 5 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 480);
	
	return result;	
}

int Test_VSMAFindLargestFree()
{
	int result = 1;
	size_t test_no = 0;
	int res = 0;
	vsma_t *vsma = (vsma_t *)malloc(500 * sizeof(char));
	vsma_t *alloc1 = NULL;
	vsma_t *alloc2 = NULL;
	memset(vsma, 5, 464);		 
	vsma = VSMAInit(vsma, 500);	
	
	/* test 1 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 480);
	
	alloc1 = VSMAAlloc(vsma, 199);
	
	/* test 2 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 272);
	
	alloc2 = VSMAAlloc(vsma, 100);
	
	/* test 3 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 160);

	VSMAFree(alloc2);
	
	/* test 4 */
	res = VSMAFindLargestFree(vsma);
	TEST_EQUAL(res, 272);
	
	alloc2 = VSMAAlloc(vsma, 100);	
	VSMAFree(alloc1);
	
	/* test 5 */
	res = VSMAFindLargestFree(vsma);
	TEST_EQUAL(res, 200);

	VSMAFree(alloc2);
	
	/* test 6 */
	res = VSMAFindLargestFree(vsma);
	TEST_EQUAL(res, 480);
	
	return result;	
}

int Test_VSMAFree()
{
	int result = 1;
	size_t test_no = 0;
	int res = 0;
	vsma_t *vsma = (vsma_t *)malloc(500 * sizeof(char));
	vsma_t *alloc1 = NULL;
	vsma_t *alloc2 = NULL;
	memset(vsma, 5, 464);		 
	vsma = VSMAInit(vsma, 500);	
	alloc2 = VSMAAlloc(vsma, 480);
	
	/* test 1*/
	res = VSMACount(vsma);
	TEST_EQUAL(res, 0);

	VSMAFree(alloc2);
	
	/* test 2*/
	res = VSMACount(vsma);
	TEST_EQUAL(res, 480);	
	return result;
}

int Test_Flow1()
{
	int result = 1;
	size_t test_no = 0;
	int res = 0;
	vsma_t *vsma = (vsma_t *)malloc(1000 * sizeof(char));
	vsma_t *alloc1 = NULL;
	vsma_t *alloc2 = NULL;
	memset(vsma, 5, 968);		 
	vsma = VSMAInit(vsma, 1000);	
	alloc1 = VSMAAlloc(vsma, 200);
	
	/* test 1*/
	res = VSMACount(vsma);
	TEST_EQUAL(res, 776);
	
	alloc2 = VSMAAlloc(vsma, 100);
	
	/* test 2*/
	res = VSMACount(vsma);
	TEST_EQUAL(res, 664);
	
	VSMAFree(alloc1);
	
	/* test 3*/
	res = VSMACount(vsma);
	TEST_EQUAL(res, 864);
	
	alloc1 = VSMAAlloc(vsma, 200);
	
	/* test 4*/
	res = VSMACount(vsma);
	TEST_EQUAL(res, 664);

	return result;
}

int DTest_VSMAInit()
{
	int result = 1;
	size_t test_no = 0;
	int res = 0;
	vsma_t *vsma = (vsma_t *)malloc(500 * sizeof(char));
	memset(vsma,5,464);
	vsma = VSMAInit(vsma, 500);
		
	/* test 1 */
	res = *(size_t *)((char *)vsma);
	TEST_EQUAL(res, 464);
		
	/* test 2 */
	res = *(size_t *)((char *)vsma + 464);
	TEST_EQUAL(res, 0);	
	
	return result;
}

int DTest_VSMAAlloc()
{
	int result = 1;
	size_t test_no = 0;
	int res = 0;
	vsma_t *vsma = (vsma_t *)malloc(500 * sizeof(char));
	vsma_t *temp = NULL; 
	memset(vsma, 5, 464);
	vsma = VSMAInit(vsma, 500);

	temp = VSMAAlloc(vsma, 199);	
	
	/* test 1 */
	res = *(size_t *)((char *)temp - 16);
	TEST_EQUAL(res, -200);
	
	/* test 2 */	
	res = *(size_t *)((char *)temp + 200);
	TEST_EQUAL(res, 248);
	
	/* test 3 */	
	res = *(size_t *)((char *)vsma + 216);
	TEST_EQUAL(res, 248);
	
	/* test 4 */
	res = *(size_t *)((char *)vsma );
	TEST_EQUAL(res, -200);
	
	temp = VSMAAlloc(vsma, 100);	
	
	/* test 5 */
	res = *(size_t *)((char *)temp - 16);
	TEST_EQUAL(res, -104);
	
	/* test 6 */
	res = *(size_t *)((char *)vsma);
	TEST_EQUAL(res, -200);
	
	/* test 7 */
	res = *(size_t *)((char *)temp + 104);
	TEST_EQUAL(res, 128);	
	
	/* test 8 */
	res = *(size_t *)((char *)vsma + 336);
	TEST_EQUAL(res, 128);
	
	return result;
}

int DTest_VSMACount()
{
	int result = 1;
	size_t test_no = 0;
	int res = 0;
	vsma_t *vsma = (vsma_t *)malloc(500 * sizeof(char));
	vsma_t *alloc1 = NULL;
	vsma_t *alloc2 = NULL;	 
	memset(vsma, 5, 464);
	vsma = VSMAInit(vsma, 500);	
	
	/* test 1 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 464);
	
	alloc1 = VSMAAlloc(vsma, 199);
	
	/* test 2 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 248);
	
	alloc2 = VSMAAlloc(vsma, 100);
	
	/* test 3 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 128);
	
	VSMAFree(alloc1);
	
	/* test 4 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 328);
	
	VSMAFree(alloc2);
	
	/* test 5 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 464);
	
	return result;	
}

int DTest_VSMAFindLargestFree()
{
	int result = 1;
	size_t test_no = 0;
	int res = 0;
	vsma_t *vsma = (vsma_t *)malloc(500 * sizeof(char));
	vsma_t *alloc1 = NULL;
	vsma_t *alloc2 = NULL;
	memset(vsma, 5, 464);		 
	vsma = VSMAInit(vsma, 500);	
	
	/* test 1 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 464);
	
	alloc1 = VSMAAlloc(vsma, 199);
	
	/* test 2 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 248);
	
	alloc2 = VSMAAlloc(vsma, 100);
	
	/* test 3 */
	res = VSMACount(vsma);
	TEST_EQUAL(res, 128);

	VSMAFree(alloc2);
	
	/* test 4 */
	res = VSMAFindLargestFree(vsma);
	TEST_EQUAL(res, 248);
	
	alloc2 = VSMAAlloc(vsma, 100);	
	VSMAFree(alloc1);
	
	/* test 5 */
	res = VSMAFindLargestFree(vsma);
	TEST_EQUAL(res, 200);

	VSMAFree(alloc2);
	
	/* test 6 */
	res = VSMAFindLargestFree(vsma);
	TEST_EQUAL(res, 464);
	
	return result;	
}

int DTest_VSMAFree()
{
	int result = 1;
	size_t test_no = 0;
	int res = 0;
	vsma_t *vsma = (vsma_t *)malloc(500 * sizeof(char));
	vsma_t *alloc1 = NULL;
	vsma_t *alloc2 = NULL;
	memset(vsma, 5, 464);		 
	vsma = VSMAInit(vsma, 500);	
	alloc2 = VSMAAlloc(vsma, 464);
	
	/* test 1*/
	res = VSMACount(vsma);
	TEST_EQUAL(res, 0);

	VSMAFree(alloc2);
	
	/* test 2*/
	res = VSMACount(vsma);
	TEST_EQUAL(res, 464);	
	return result;
}

int DTest_Flow1()
{
	int result = 1;
	size_t test_no = 0;
	int res = 0;
	vsma_t *vsma = (vsma_t *)malloc(1000 * sizeof(char));
	vsma_t *alloc1 = NULL;
	vsma_t *alloc2 = NULL;
	memset(vsma, 5, 968);		 
	vsma = VSMAInit(vsma, 1000);	
	alloc1 = VSMAAlloc(vsma, 200);
	
	/* test 1*/
	res = VSMACount(vsma);
	TEST_EQUAL(res, 752);
	
	alloc2 = VSMAAlloc(vsma, 100);
	
	/* test 2*/
	res = VSMACount(vsma);
	TEST_EQUAL(res, 632);
	
	VSMAFree(alloc1);
	
	/* test 3*/
	res = VSMACount(vsma);
	TEST_EQUAL(res, 832);
	
	alloc1 = VSMAAlloc(vsma, 200);
	
	/* test 4*/
	res = VSMACount(vsma);
	TEST_EQUAL(res, 632);

	return result;
}
