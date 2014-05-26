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
 * Calcuate the average of the elements of array[] from begin to end. If begin
 * or end fall out of bounds return DBL_MAX
 */
double
average(int begin, int end, int array[], const int size)
{
    double sum = 0;
    double average;
    
    // Do not exceed array
    if (begin < 0 || end >= size)
    {
        return DBL_MAX;
    }
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
    if (begin < 0 || end >= size)
    {
        return DBL_MAX;
    }
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