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

#define WIDTH 5

// Declare functions
double average(int begin, int end, int array[], int size);
int runningAverage(int start, int end, int array[], int size);

int
main (void)
{
    int array[15] = {550,516,534,531,599,577,543,480,453,419,400,371,356,332,
                       200};
    int start;

    printf("The number is [%i]\n", runningAverage(0, WIDTH, array, 15));
}

double
average(int begin, int end, int array[10], int size)
{
    double sum = 0;
    double average;
    
//    if (end > (size - 1))
//    {
//        begin -= (end - (size - 1));
//        end = (size - 1);
//    }

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
    
    return (average);
}

int
runningAverage(int start, int end, int array[], int size)
{
    if (average(start, end, array, size) < average(start + WIDTH, end + WIDTH, array, size))
        return (start);
    else if ((end + WIDTH) == size)
    {
        // We reached end of array and we know this average is the lowest
        // so we return the starting point of the final sequence
        return (start + WIDTH);
    }

    start += WIDTH;
    end += WIDTH;

    // DEBUGGING
    printf("=======================\n");
    printf("start = %i\n", start);
    printf("end = %i\n", end);
    printf("=======================\n");
        
    return (runningAverage(start, end, array, size));
}
