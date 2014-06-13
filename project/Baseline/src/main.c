/* 
 * ============================================================================
 * 
 *			Filename:	main.c
 *		 Description:	This program calculates the average (baseline) of an
 *						array of value's up to the start of a significant
 *						spike.
 *
 *			 Version:	1.0
 *			 Created:	06-06-2014 16:32:16
 *
 *			  Author:	Jorian van Oostenbrugge (jorianvo)
 *
 *			   Notes:	Call Library source file. Using labview compatible
 *						types (int32_t and uint16_t) and headers (extcode.h)
 *
 * ============================================================================
 */

#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <math.h>

#include "extcode.h"
#include "sequence.h"

// Function declarations
_declspec (dllexport) int32_t findBaseline(int32_t start, int32_t end, 
	uint16_t array[], const int32_t size, const uint16_t threshold, 
	const int32_t width, int16_t *pBaseline, int16_t *pStdev);
int32_t calculateBaseline(int32_t start, int32_t end, uint16_t array[],
	const int32_t size, const uint16_t threshold, int16_t *pBaseline,
	int16_t *pStdev);
bool inRange(const int32_t threshold, double value);


_declspec (dllexport) int32_t findBaseline(int32_t start, int32_t end, 
	uint16_t array[], const int32_t size, const uint16_t threshold, 
	const int32_t width, int16_t *pBaseline, int16_t *pStdev)
{
	// Error checking
	if (size < 0 || width < 0)
		return (-5000);

	// Try to calculate the baseline starting from start. If it fails return
	// element of error i.e. starting point of error
	int32_t startOfError = calculateBaseline(start, end, array, size, 
											 threshold, pBaseline, pStdev);

	// If we find a baseline exit cleanly, else everything below -1 signifies 
	// error so return generated error
	if (startOfError == -1)
		return 0;
	else if (startOfError < -1)
		return startOfError;

	// No baseline yet, so find next starting position, startting from
	// startOfError
	int32_t newStart = compareSequences(startOfError, (startOfError + width), 
										array, size, width);

	// No next starting point...
	if (newStart == INT_MAX)
	{
		*pBaseline = -999;
		*pStdev = -999;
		return -999;
	}

	// No problems so far, so update the end and try again
	int32_t newEnd = newStart + width;

	return (findBaseline(newStart, newEnd, array, size, threshold, width,
						 pBaseline, pStdev));
}

/*
* This function tries to determine the baseline from a given array.
* If we do not have sufficient points before we reach the spike, the
* function returns the element were if failed else if we do find
* enough points, we set a pointer to the value of the calculated
* baseline and return -1 because that can never be an element of an
* array
*/
int32_t
calculateBaseline(int32_t start, int32_t end, uint16_t array[], 
					const int32_t size, const uint16_t threshold,
					int16_t *pBaseline, int16_t *pStdev)
{
	// Declare variables
	int32_t i;
	double sum = 0;
	double average;
	double differenceFromAverage;
	double differenceInPoints;

	// Make sure it is likely we have more than enough points to calculate the 
	// baseline
	if (start < 0 || end > size)
		return (-5001);
	else if (start == end)
		return (-5002);
	else if ((end - start) < 100)
		return (-5003);

	// Itereate over each element in the array
	// Start with second element (start + 1) because we need to compare it to a
	// previous element. We want as much elements as possible so we set size of
	// array as maximum. We do not need more than 'end - start' elements to
	// accurately determine the baseline, the +1 accounts for the fact that we
	// take the average until i - 1, which then translates to 'start' and 'end'
	for (i = (start + 1); i < (end + 1); i++)
	{
		// Calculate the average up to i.e. not including the current element
		// Should go before threshold check, otherwise last element is not
		// included in baseline
		sum += array[i - 1];
		average = sum / (i - start);

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

	// If we have enough points to calculate the baseline set pointer to
	// baseline value and return (-1) else return element were calculating
	// failed. We want al least 95% of bins iterated to be used for the 
	// baseline 
	if ((i - start) >= 95)
	{
		// Return -1 means everthing went ok and we've found a baseline 
		*pBaseline = (int16_t) round(average);

		// In order to comply with the old baseline filter return
		// stdev times 1000
		double miliStdev = stdev(start, (i - 1), array, average, size);
		*pStdev = (int16_t) (miliStdev * 1000);

		return (-1);
	}
	else
		return (i);
}

/*
* Check if a value lies within the absolute value of the supplied threshold
* i.e. more than the negative value of the threshold and less than the
* positive value of the threshold
*/
bool
inRange(const uint16_t threshold, double value)
{
	if (threshold >= value && value >= (-threshold))
		return true;
	else
		return false;
}