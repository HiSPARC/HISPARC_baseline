/*
 * ============================================================================
 *
 *       Filename:  test-average.c
 *
 *    Description:  A testing module to test the the outcome of the stdev
 *                  function defined in '../../src/sequence.c' agianst known
 *                  value's
 *
 *        Version:  1.0
 *        Created:  21-05-2014 14:34:29
 *       Compiler:  gcc
 *
 *         Author:  Jorian van Oostenbrugge
 *
 * ============================================================================
 */

#include <stdio.h>
#include <float.h>
#include "../../src/sequence.h"
#include "../minunit.h"

// Global variables
int tests_run = 0;
double stdevAll;
double negativeAll;

// Define tests
// Test full stdev
static char * testFullstdev()
{
    mu_assert("error, stdev of full array not correct", stdevAll == 1.0);
    return 0;
}

// Test stdev with negative numbers
static char * testNegativeStdev()
{
    mu_assert("error, stdev of negative values not correct", negativeAll == 2);
    return 0;
}

// All tests
static char * all_tests() 
{
    mu_run_test(testFullstdev);
    mu_run_test(testNegativeStdev);
    return 0;
}
 
int 
main(void) 
{
    // Array's for testing
    int array[8]={5, 7, 5, 7, 5, 7, 7, 5};
    int negativeArray[4] = {-2, -6, -6, -2};
    
    // Calculate stdev of full array
    double averageAll = average(0, 8, array, 8);
    stdevAll = stdev(0, 8, array, averageAll, 8);
    
    // Calculate average with negative value's
    double negativeValues = average(0, 4, negativeArray, 4);
    negativeAll = stdev(0, 4, negativeArray, negativeValues, 4);
    
    // Run tests
    char *result = all_tests();
    if (result != 0) 
    {
        printf("%s\n", result);
    }
    else 
    {
        printf("ALL TESTS PASSED\n");
    }
        
    printf("Tests run: %d\n", tests_run);
    return result != 0;
}

