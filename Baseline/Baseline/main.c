/* 
 * ============================================================================
 * 
 *			Filename:	main.c
 *		 Description:	This program calculates the average (baseline) of an
 *						array of value's up to the start of a significant
 *						spike.
 *
 *			 Version:	0.1
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
bool inRange(const int32_t threshold, int16_t value);

_declspec (dllexport) int32_t determine_baseline(uint16_t adValues[], 
	const int32_t size, const int32_t threshold, double *baseline)
{
	// Declare variables
	int32_t i;
	int16_t difference_points;

	// Make sure it is likely we have enough points to calculate the baseline
	if (size < 50)
		return (5001);
	
	// Go through all elements of the array
	// Start with second element (1) because we need to compare it to a 
	// previous element
	for (i = 1; i < size; i++)
	{
		// Determine difference between current and previous point
		difference_points = (signed int) adValues[i] - adValues[i - 1];

		// Make sure points are within the threshold
		if (!inRange(threshold, difference_points))
			break;
	};

	// We need at least fifty points to accurately determine baseline
	if (i >= 50)
	{
		*baseline = 10.0;
		return 0;
	}
	else
		return (5002);
}

/*
 * Check if a value lies within the absolute value of the supplied threshold
 * i.e. more than the negative value of the threshold and less than the
 * positive value of the threshold
 */

bool
inRange(const int32_t threshold, int16_t value)
{
	if (threshold >= value && value >= (-threshold))
		return true;
	else
		return false;
}