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
 *        Created:  06-06-2014 17:24:09
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jorian van Oostenbrugge (jorianvo)
 *
 *                  copyright (c) 2014
 *
 * ============================================================================
 */
#include "extcode.h"

#ifndef _SEQUENCE_H
#define _SEQUENCE_H

#define STDEVLIMIT 4
#define MAXAVERAGE 245
#define SEQUENCEWIDTH 100

// Declare struct to hold sequence properties
struct sequence
{
    double average;
    double stdev;
};

/*
 * Calculate properties of array: average of elements from begin to end and
 * the standard deviation and return a struct with these properties.
 */
struct sequence calculateProperties(int32_t begin, int32_t end, 
										uint16_t array[], const int32_t size);

/*
 * Calcuate the average of the elements of array[] from begin to end 
 * (not including end). If begin or end fall out of bounds return DBL_MAX
 */
double average(int32_t begin, int32_t end, uint16_t array[], 
				const int32_t size);

/*
 * Calculate the standard deviation of the elements of array[] from
 * begin to end using average. If begin or end falls out of bounds 
 * return DBL_MAX
 */
double stdev(int32_t begin, int32_t end, uint16_t array[], 
				const double average, const int32_t size);

/*
 * Determine the starting point for determining the baseline the start in
 * combination with the end define the width of the "sequence" i.e. an array
 * which starts at start and ends at end - 1. This function compares two
 * such sequences and stops until it finds a sequence with a smoother baseline
 * compared to the next sequence. If the exists no such sequence an error
 * with value INT_MAX is returned
 */
int32_t compareSequences(int32_t startFirstSequence, int32_t endFirstSequence, 
						 uint16_t array[], const int32_t size, 
						 const int32_t width);

#endif
