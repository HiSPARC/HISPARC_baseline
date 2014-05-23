/* 
 * ============================================================================
 * 
 *			Filename:	main.c
 *		 Description:	This program calculates the average (baseline) of an
 *						array of value's up to the start of a significant
 *						spike.
 *
 *			 Version:	0.9
 *			 Created:	14-05-2014 16:20:23
 *
 *			  Author:	Jorian van Oostenbrugge (jorianvo)
 *
 *			   Notes:	Call Library source file. Using labview compatible
 *						types (int32_t and uint16_t) and headers (extcode.h)
 *
 * ============================================================================
 */

#include "extcode.h"
#include "stdbool.h"

// Function declarations
_declspec (dllexport) int32_t determine_baseline(uint16_t adValues[], 
	const int32_t size, const int32_t threshold, double *baseline);
bool inRange(const int32_t threshold, double value);

_declspec (dllexport) int32_t determine_baseline(uint16_t adValues[], 
	const int32_t size, const int32_t threshold, double *baseline)
{
	// Declare variables
	int32_t i;
	double sum = 0;
	double average;
	double differenceFromAverage;
	double differenceInPoints;

	// Make sure it is likely we have enough points to calculate the baseline
	if (size < 50)
		return (-5001);
	
	// Go through all elements of the array
	// Start with second element (1) because we need to compare it to a 
	// previous element
	for (i = 1; i < size; i++)
	{
		// Calculate the average
		// Should go before threshold check, otherwise last element is not
		// included in baseline
		sum += adValues[i - 1];
		average = sum / i;

		// Determine difference between average and current point
		differenceFromAverage = (double) adValues[i] - average;

		// Deviaton from average should fall within threshold
		if (!inRange(threshold, differenceFromAverage))
			break;
		
		// Determine difference between current and previous point
		differenceInPoints = (double) adValues[i] - adValues[i - 1];

		// Make sure separate points are within the double threshold
		if (!inRange((threshold * 2), differenceInPoints))
			break;
	};

	// We need at least fifty points to accurately determine baseline
	if (i >= 50)
	{
		*baseline = average;
		return 0;
	}
	else
		return (-5002);
}

/*
 * Check if a value lies within the absolute value of the supplied threshold
 * i.e. more than the negative value of the threshold and less than the
 * positive value of the threshold
 *
 */

bool
inRange(const int32_t threshold, double value)
{
	if (threshold >= value && value >= (-threshold))
		return true;
	else
		return false;
}