/*
 * ============================================================================
 *
 *       Filename:  test-average.c
 *
 *    Description:  A testing module to test the the outcome of the average 
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
double averageAll;
double averagePart;
double outOfBounds;
double negativeValues;

// Define tests
// Test full average
static char * testFullAverage()
{
    mu_assert("error, average of full array not correct", averageAll == 9.2);
    return 0;
}

// Test average of part of array
static char * testPartAverage()
{
    mu_assert("error, average of part of array not correct", averagePart == 3.5);
    return 0;
}

// Test out of bounds
static char * testOutOfBounds()
{
    mu_assert("error, out of bounds not correct", outOfBounds == DBL_MAX);
    return 0;
}

// Test with negative numbers
static char * testNegative()
{
    mu_assert("error, negative values not correct", negativeValues == -0.4);
    return 0;
}

// All tests
static char * all_tests() 
{
    mu_run_test(testFullAverage);
    mu_run_test(testPartAverage);
    mu_run_test(testOutOfBounds);
    mu_run_test(testNegative);
    return 0;
}
 
int 
main(void) 
{
    // Array's for testing
    int array[10]={5, 6, 3, 8, 11, 2, 1, 0, 13, 43};
    int negativeArray[5] = {-2, -8, 4, -1, 5};
    
    // Calculate average of full array
    averageAll = average(0, 10, array, 10);
    
    // Calculate average of part of array
    averagePart = average(4, 8, array, 10);
    
    // Calculate average if out of bounds
    outOfBounds = average(0, 20, array, 10);
    
    // Calculate average with negative value's
    negativeValues = average(0, 5, negativeArray, 5);
    
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

