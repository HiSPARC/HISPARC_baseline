/*
 * ============================================================================
 *
 *       Filename:  sequence.h
 *
 *    Description:  Header file containing all function declarations for 
 *                  functions implemented in sequence.c. Used to find a good
 *                  starting point for determining the baseline
 *
 *        Version:  1.0
 *        Created:  23-05-2014 15:25:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jorian van Oostenbrugge (jorianvo)
 *
 *                  copyright (c) 2014
 *
 * ============================================================================
 */

#ifndef _SEQUENCE_H
#define _SEQUENCE_H

// Declare struct to hold array properties
struct sequence
{
    double average;
    double stdev;
};

/*
 * Calculate properties of array: average of elements from begin to end and
 * the standard deviation and return a struct with these properties
 */
struct sequence calculateProperties(int begin, int end, int array[], int size);

/*
 * Calcuate the average of the elements of array[] from begin to end
 */
double average(int begin, int end, int array[]);

/*
 * Calculate the standard deviation of the elements of array[] from
 * begin to end using average
 */
double stdev(int begin, int end, int array[], const double average);

#endif
