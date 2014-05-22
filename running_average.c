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
#include <math.h>

#define WIDTH 8
#define SIZE 40
#define LIMIT 3

// Declare functions
double average(int begin, int end, int array[]);
int runningAverage(int start, int end, int array[], int size);
struct sequence calculateProperties(int begin, int end, int array[], int size);
double stdev(int begin, int end, int array[], const double average);

// Declare struct to hold array properties
struct sequence
{
    double average;
    double stdev;
};


int
main (void)
{
    int array[SIZE] = {1, 2, 7, 15, 7, 2, 3, 1, 1, 11, 1, 1, 1, 1, 1, 1, 1, 1, 
                       1, 1, 30, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 2,
                       1, 1, 1};
               
    printf("Value = %i\n", runningAverage(0, WIDTH, array, SIZE));
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

    // Calculate sample standard deviation of array
    properties.stdev = stdev(begin, end, array, properties.average);

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


/* 
 * Calculate the standard deviation of the elements of array[] from 
 * begin to end using average
 */

double
stdev(int begin, int end, int array[], const double average)
{
    double total = 0;
    double variance;
    double stdev;

    // Calculate the variance
    for (int i = begin; i < end; i++)
    {
        // Calculate sum of squares
        total += pow((array[i] - average), 2.0);
    }
    
    // Average sum of squares
    variance = total / WIDTH;

    stdev = sqrt(variance);

    return (stdev);
}


// DO NOT NEED RIGHT NOW
int
runningAverage(int start, int end, int array[], int size)
{
    int nextStart, nextEnd;
    struct sequence currentSequence, nextSequence;

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

    // set sequences
    currentSequence = calculateProperties(start, end, array, SIZE);
    nextSequence = calculateProperties(nextStart, nextEnd, array, SIZE);

    if (end == size)
    {
        // Array exactly size of sequence
        return start;
    }
    else if (currentSequence.stdev < LIMIT)
    {
         if (currentSequence.stdev < nextSequence.stdev)
         {
             // A lower stdev i.e. a smoother line is more important than a low
             // average so return
             return (start);
         }
         else if (currentSequence.stdev == nextSequence.stdev)
         {
             if (currentSequence.average < nextSequence.average)
             {
                 return (start);
             }
         }
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
