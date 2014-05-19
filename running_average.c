/*
 * ============================================================================
 *
 *       Filename:  running_average.c
 *
 *    Description:  This program implements a running average to still be able
 *                  to determine the average even if we have to few points
 *                  before the spike
 *
 *        Version:  1.0
 *        Created:  16-05-2014 15:06:53
 *       Compiler:  gcc
 *
 *         Author:  Jorian van Oostenbrugge
 *
 * ============================================================================
 */

#include <stdio.h>

#define WIDTH 10

// Declare functions
double average(int begin, int end, int array[], int size);
int runningAverage(int start, int end, int array[], int size);

int
main (void)
{
    int array[64] = {208,207,208,206,210,206,207,204,212,203,210,204,208,204,
        206,202,208,203,206,202,208,202,205,201,205,200,206,200,200,208,198,
        201,211,203,204,196,202,203,204,197,205,202,203,200,204,200,204,200,
        205,199,202,201,205,199,202,200,200,200,200,200,207,201,203,200};
    int start;

    printf("The number is [%i]\n", runningAverage(0, WIDTH, array, 11));
}

double
average(int begin, int end, int array[10], int size)
{
    double sum = 0;
    double average;

    for (int i = begin; i < end; i++)
    {
        sum += array[i];
    }

    average = sum / WIDTH;
    
    // DEBUGGING
    printf("begin = %i\n", begin);
    printf("end = %i\n", end);
    printf("average = %f\n", average);
    printf("____________________________\n");
    
    return average;
}

int
runningAverage(int start, int end, int array[], int size)
{
    if (average(start, end, array, size) >= average(start + WIDTH, end + WIDTH, array, size))
    {
        start += WIDTH;
        end += WIDTH;

        // DEBUGGING
        printf("=======================\n");
        printf("start = %i\n", start);
        printf("end = %i\n", end);
        printf("=======================\n");
        
        runningAverage(start, end, array, size);
    }

    // DEBUGGING
    printf("start = %i\n", start);
    return start;
}
