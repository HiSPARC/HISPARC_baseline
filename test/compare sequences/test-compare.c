/*
 * ============================================================================
 *
 *       Filename:  test-average.c
 *
 *    Description:  A testing module to test the the outcome of the 
 *                  compareSequence function defined in '../../src/sequence.c'
 *                  agianst a known predefined array
 *
 *        Version:  1.0
 *        Created:  04-06-2014 13:24:09
 *       Compiler:  gcc
 *
 *         Author:  Jorian van Oostenbrugge
 *
 * ============================================================================
 */

#include <stdio.h>
#include <limits.h>
#include "../../src/sequence.h"
#include "../minunit.h"

// Global variables
int tests_run = 0;
int compare;
int wrongEnd;
int shift;

// Define tests
// Test full average
static char * testCompare()
{
    mu_assert("error, comparing sequences return wrong number", compare == 100);
    return 0;
}

// Test wrong end
static char * testWrongEnd()
{
    mu_assert("error, the wrong end should return INT_MAX", wrongEnd == INT_MAX);
    return 0;
}

// Test shifting of sequence to fit array
static char * testShift()
{
    mu_assert("error, the shifting isn't working a it should", shift == 110);
    return 0;
}

// All tests
static char * all_tests()
{
    mu_run_test(testCompare);
    mu_run_test(testWrongEnd);
    mu_run_test(testShift);
    return 0;
}

int
main(void)
{
    // Array's for testing
    int array[400]={203,198,201,198,200,197,198,197,202,197,201,196,198,197,
                    202,199,201,199,198,196,200,199,202,197,202,195,198,197,
                    202,199,202,199,201,198,199,198,199,197,202,197,199,196,
                    201,196,198,200,202,199,203,198,200,199,200,198,199,198,
                    202,197,201,197,202,198,200,198,201,198,200,199,202,198,
                    200,198,201,200,201,197,201,198,200,198,201,199,200,197,
                    201,197,199,197,202,198,202,197,203,199,201,196,203,199,
                    201,195,201,197,200,199,199,196,200,197,203,197,199,198,
                    201,198,201,197,200,198,197,196,201,197,201,197,200,199,
                    201,196,203,197,199,200,200,196,200,199,203,198,200,198,
                    202,199,201,197,200,198,202,199,201,198,199,197,202,199,
                    199,197,202,198,202,198,199,196,200,199,200,198,200,197,
                    202,197,200,198,200,198,199,198,203,197,200,197,200,197,
                    202,198,200,196,201,200,201,200,201,199,201,197,199,198,
                    202,197,200,197,202,197,202,198,199,198,200,198,199,197,
                    203,197,203,197,198,197,202,197,200,196,199,197,201,195,
                    201,198,202,197,201,196,200,197,200,198,202,199,199,198,
                    201,196,199,197,202,199,203,199,200,198,201,198,203,197,
                    200,198,199,197,199,198,202,198,200,198,199,199,201,198,
                    200,197,200,197,199,196,202,199,199,199,200,196,198,198,
                    201,197,202,196,200,198,202,199,203,197,197,199,201,197,
                    200,199,201,196,199,198};
    
    // Compare two sequences until the end of the array is reached
    compare = compareSequences(0, 100, array, 300, 100);
    
    // Set the end on purpose much to high
    wrongEnd = compareSequences(0, 400, array, 300, 400);
    
    // Set width to such a number that we need to shift the end
    shift = compareSequences(0, 110, array, 300, 110);
    
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


