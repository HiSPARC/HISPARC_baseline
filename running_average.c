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
#include <limits.h>

#define WIDTH 100
#define SIZE 269
#define LIMIT 3
#define MAXAVERAGE 245

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
    int array[SIZE] = {238,312,427,519,539,524,489,449,413,386,365,357,347,328,
        317,299,285,274,279,271,258,251,243,239,245,238,237,228,
        223,218,217,214,214,214,214,211,216,214,214,211,213,212,
        211,205,209,204,208,209,216,213,214,208,208,208,207,204,
        209,205,207,202,203,214,225,218,217,212,215,208,206,206,
        205,201,203,201,204,201,202,199,200,198,203,199,203,202,
        203,200,203,198,205,199,203,197,202,199,202,199,204,199,
        203,203,206,203,203,207,218,215,212,207,211,205,205,202,
        202,200,203,200,201,200,205,199,202,199,201,200,200,200,
        202,199,203,200,197,200,202,201,201,200,203,199,201,199,
        201,199,198,200,204,199,202,199,201,200,201,198,201,200,
        201,198,204,199,198,197,199,196,199,198,201,197,203,198,
        200,197,202,197,198,198,198,197,201,197,198,198,200,196,
        199,199,200,196,198,198,200,196,201,195,200,197,200,198,
        197,197,201,197,199,197,199,195,199,197,199,197,200,198,
        203,197,199,198,203,197,201,197,210,215,221,195,198,195,
        199,196,202,199,198,199,201,198,201,199,200,198,199,198,
        200,199,199,195,202,197,199,197,201,196,201,199,201,197,
        197,197,200,195,201,197,200,200,201,211,232,236,232,222,
        221,209,209};
               
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
        // if it falls below limits return start else return error
        if (currentSequence.average < MAXAVERAGE && 
              currentSequence.stdev < LIMIT)
            return start;
        else
            return (INT_MAX);
    }
    else if (currentSequence.stdev < LIMIT)
    {
        // stdev falls within limit so first check if theere is no overflow
        if (currentSequence.average < MAXAVERAGE && 
            nextSequence.average < MAXAVERAGE)
        {
            if (currentSequence.stdev < nextSequence.stdev)
            {
                // A lower stdev i.e. a smoother line is more important than a 
                // low average so return
                return (start);
            }
            else if (currentSequence.stdev == nextSequence.stdev)
            {
                // Because both stdevs are equal we want the lowest average
                if (currentSequence.average < nextSequence.average)
                    return (start);
            }
        }
        else if (nextSequence.average > MAXAVERAGE)
        {
            // encountered an overflow if this is the last sequence of the
            // array return an error INT_MAX because all preceeding
            // sequences also had problems else if it is not the end of the
            // array go on because chances are great we comes across a better
            // sequence
            if (nextEnd == size)
            {
                if (currentSequence.average < MAXAVERAGE)
                    return (start);
                else
                    return (INT_MAX);
            }
        }
    }
    else if (nextEnd == size)
    {
        // A whole number of sequences fits in the array and the last sequence 
        // looks like the best baseline, so if it falls below all limits return
        // the startvalue else return error
        if (nextSequence.average < MAXAVERAGE &&
              nextSequence.stdev < LIMIT)
            return (nextStart);
        else
            return (INT_MAX);
    }

    start += WIDTH;
    end += WIDTH;

    return (runningAverage(start, end, array, size));
}