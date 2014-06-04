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
#include <stdbool.h>
#include "sequence.h"

#define WIDTH 100
#define SIZE 1934
#define STDEVLIMIT 3
#define MAXAVERAGE 245

// Declare functions
bool inRange(const int threshold, double value);
int compareSequences(int start, int end, int array[], int size);

int
main (void)
{
    printf("Value = %i\n", compareSequences(0, WIDTH, array, SIZE));
}

/*
 * This function tries to determine the baseline from a given array.
 * If we do not have sufficient points before we reach the spike, the
 * function returns -1 else if we do find enough points, we return a
 * pointer to the value of the calculated baseline and return 0
 */
int
determineBaseline(int start, int end, int array[], int size, const int threshold, double *baseline)
{
    // Declare variables
    int i;
    double sum = 0;
    double average;
    double differenceFromAverage;
    double differenceInPoints;
    
    // Make sure it is likely we have more than enough points to calculate the baseline
    if (size < 50)
        return (-5001);
    
    // Itereate over each element in the array
    // Start with second element (i = 1) because we need to compare it to a previous element
    for (i = 1; i < size; i++)
    {
        // Calculate the average up to i.e. not including the current element
		// Should go before threshold check, otherwise last element is not
		// included in baseline
        sum += array[i - 1];
        average = sum / i;
        
        // Determine difference between average and current point
        // Should go before difference between current point and previous point
        // because this is more likely to fail first
		differenceFromAverage = (double) array[i] - average;
        
		// Deviaton from average should fall within threshold
		if (!inRange(threshold, differenceFromAverage))
			break;
        
        // Determine difference between current and previous point
		differenceInPoints = (double) array[i] - array[i - 1];
        
		// Make sure separate points are within the double threshold
		if (!inRange((threshold * 2), differenceInPoints))
			break;
    }
    
    // IF we have enough points to calculate the baseline set pointer to
    // baseline value and quit
    if (i >= 50)
    {
        *baseline = average;
        return 0;
    }
    else
        return (-1);
}

/*
 * Check if a value lies within the absolute value of the supplied threshold
 * i.e. more than the negative value of the threshold and less than the
 * positive value of the threshold
 */
bool
inRange(const int threshold, double value)
{
	if (threshold >= value && value >= (-threshold))
		return true;
	else
		return false;
}