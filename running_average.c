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

#define WIDTH 8

// Declare functions
double average(int begin, int end, int array[], int size);
int runningAverage(int start, int end, int array[], int size);

int
main (void)
{
    int array[15] = {550,516,534,531,599,577,543,480,453,419,400,371,356,332,
                       200};

    printf("The number is [%i]\n", runningAverage(0, WIDTH, array, 15));
}

double
average(int begin, int end, int array[], int size)
{
    double sum = 0;
    double average;
    
    // Calculate the average
    for (int i = begin; i < end; i++)
    {
        sum += array[i];
    }

    average = sum / WIDTH;
    
    return (average);
}

int
runningAverage(int start, int end, int array[], int size)
{
    int nextStart, nextEnd;

    // Do not exceed array
    if ((end + WIDTH) > size)
    {
        nextStart = (start + WIDTH) - ((end + WIDTH) - size);
        nextEnd = size;
    }
    else
    {
        nextStart = start + WIDTH;
        nextEnd = end + WIDTH;
    }

    if (end == size)
    {
        // Array exactly size of sequence
        return start;
    }
    else if (average(start, end, array, size) < average(nextStart, nextEnd, array, size))
    {
        // The current sequence has a lower average than the next so return
        return (start);
    }
    else if (nextEnd == size)
    {
        // A whole number of sequences fits in the array and the last sequence 
        // is the lowest so we return the starting point of the final sequence
        return (nextStart);
    }

    start += WIDTH;
    end += WIDTH;

    return (runningAverage(start, end, array, size));
}
