/*
 * ============================================================================
 *
 *       Filename:  sequence.c
 *
 *    Description:  This file contains the implementation of the functions
 *                  prototypes defined in sequence.h
 *
 *        Version:  1.0
 *        Created:  23-05-2014 16:09:39
 *       Compiler:  gcc
 *
 *         Author:  Jorian van Oostenbrugge (jorianvo)
 *
 *                  copyright (c) 2014
 *
 * ============================================================================
 */

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <limits.h>

#include "sequence.h"

/*
 * Calculate properties of array: average of elements from begin to end and
 * the standard deviation and return a struct with these properties
 */
struct sequence
calculateProperties(int begin, int end, int array[], const int size)
{
    struct sequence properties;
    
    // Calculate average of array
    properties.average = average(begin, end, array, size);
    
    // Calculate sample standard deviation of array
    properties.stdev = stdev(begin, end, array, properties.average, size);
    
    return properties;
}

/*
 * Calcuate the average of the elements of array[] from begin to end (not including end). 
 * If begin or end fall out of bounds return DBL_MAX
 */
double
average(int begin, int end, int array[], const int size)
{
    double sum = 0;
    double average;
    
    // Do not exceed array
    if (begin < 0 || end > size)
        return DBL_MAX;
    else if (begin == end)
        return DBL_MAX;
    
    // Calculate the average
    for (int i = begin; i < end; i++)
    {
        sum += array[i];
    }
    
    average = sum / (end - begin);
    
    return (average);
}

/*
 * Calculate the standard deviation of the elements of array[] from
 * begin to end using average. If begin or end falls out of bounds
 * return DBL_MAX
 */
double
stdev(int begin, int end, int array[], const double average, const int size)
{
    double total = 0;
    double variance;
    double stdev;
    
    // Do not exceed array
    if (begin < 0 || end > size)
        return DBL_MAX;
    else if (begin == end)
        return DBL_MAX;
    
    // Calculate the variance
    for (int i = begin; i < end; i++)
    {
        // Calculate sum of squares
        total += pow((array[i] - average), 2.0);
    }
    
    // Average sum of squares
    variance = total / (end - begin);
    
    stdev = sqrt(variance);
    
    return (stdev);
}

/*
 * Determine the starting point for determining the baseline. The start in
 * combination with the end define the width of the "sequence" i.e. an array
 * which starts at start and ends at end - 1. The width is also an argument itself
 * to avoid calculating it eacht time. This function compares two
 * such sequences and stop until it finds a sequence with a smoother baseline
 * compared to the next sequence. If there exists no such sequence an error
 * with value INT_MAX is returned. For normal usage the width should be equal
 * to the end when the function is first called. If they are not equal, we could either
 * miss elements or get an overlap of sequences.
 */
int
compareSequences(int start, int end, int array[], const int size, const int width)
{
    int nextStart, nextEnd;
    struct sequence currentSequence, nextSequence;
    
    // Do not exceed array else if possible shift sequence back to fall exactly
    // in array
    if (start < 0 || end > size)
        return (INT_MAX);
    else if (start == end)
        return (INT_MAX);
    else if ((end + width) > size)
    {
        nextStart = (start + width) - ((end + width) - size);
        nextEnd = size;
    }
    else
    {
        nextStart = start + width;
        nextEnd = end + width;
    }
    
    // Set sequences
    currentSequence = calculateProperties(start, end, array, size);
    nextSequence = calculateProperties(nextStart, nextEnd, array, size);
    
    // Get average and stdev
    double cuAverage = currentSequence.average;
    double neAverage = nextSequence.average;
    double cuStdev = currentSequence.stdev;
    double neStdev = nextSequence.stdev;
    
    // Base cases i.e. when to end the recursion
    if (end == size)
    {
        // Array exactly size of sequence
        // if it falls below limits return start else return error
        if (cuAverage < MAXAVERAGE && cuStdev < STDEVLIMIT)
            return start;
        else
            return (INT_MAX);
    }
    else if (cuStdev < STDEVLIMIT)
    {
        // Stdev falls within limit so first check if theere is no saturation
        if (cuAverage < MAXAVERAGE && neAverage < MAXAVERAGE)
        {
            if (cuStdev < neStdev)
            {
                // A lower stdev i.e. a smoother line is more important than a
                // low average so return
                return (start);
            }
            else if (cuStdev == neStdev)
            {
                // Because both stdevs are equal we want the lowest average
                if (cuAverage < neAverage)
                    return (start);
            }
            else if (nextEnd == size)
            {
                // nextSequence has lower stdev and is the last sequence so return
                return (nextStart);
            }
        }
        else if (neAverage > MAXAVERAGE)
        {
            // Encountered saturation of the adc if this is the last sequence
            // of the array return an error INT_MAX because all preceeding
            // sequences also had problems else if it is not the end of the
            // array go on because chances are great we comes across a better
            // sequence
            if (nextEnd == size)
            {
                if (cuAverage < MAXAVERAGE)
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
        // the start value else return error
        if (neAverage < MAXAVERAGE && neStdev < STDEVLIMIT)
            return (nextStart);
        else
            return (INT_MAX);
    }
    
    start += width;
    end += width;
    
    return (compareSequences(start, end, array, size, width));
}