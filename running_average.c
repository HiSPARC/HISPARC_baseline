/*
 * ============================================================================
 *
 *       Filename:  running_average.c
 *
 *    Description:  This program implements a running average to still be able
 *                  to determine the average even if we have to few points
 *                  before the spike
 *
 *        Version:  2.0
 *        Created:  16-05-2014 15:06:53
 *       Compiler:  gcc
 *
 *         Author:  Jorian van Oostenbrugge
 *
 * ============================================================================
 */

#include <stdio.h>

#define WIDTH 4
#define SIZE 14

// Declare functions
double average(int begin, int end, int array[]);
int runningAverage(int start, int end, int array[], int size);
struct sequence calculateProperties(int begin, int end, int array[], int size);

// Declare struct
struct sequence
{
    double average;
    double stdev;
};

int
main (void)
{
    int array[SIZE] = {150,216,234,531,599,577,543,480,453,419,400,371,356,332};

    // TESTING
    struct sequence currentSequence;
    currentSequence = calculateProperties(0, WIDTH, array, SIZE);

    printf("Properties of array:\n");
    printf("- average = %.4f\n", currentSequence.average);
    printf("- standard deviation = %.4f\n", currentSequence.stdev);
}


/* 
 * Calculate properties of array: average of elements from begin to end and
 * the standard deviation and return a struct with these properties
 */

struct sequence
calculateProperties(int begin, int end, int array[], int size)
{
    struct sequence properties;
    
    // Calculate average of array
    properties.average = average(begin, end, array);

    // DEBUGGING
    properties.stdev = 42.0;

    return properties;
}


/*
 * Calcuate the average of the elements of array[] from begin to end
 */

double
average(int begin, int end, int array[])
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

// DO NOT NEED RIGHT NOW
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
    else if (average(start, end, array) < average(nextStart, nextEnd, array))
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
