/* 
 * ============================================================================
 * 
 *			Filename:	main.c
 *		 Description:	This program calculates the average (baseline) of an
 *						array of value's up to the start of a significant
 *						pulse.
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
#include <string.h>

#include "extcode.h"
#include "sequence.h"

// Function declarations
_declspec (dllexport) int32_t findBaseline(int8_t *errorBoolean, 
	char *errorMessage, int32_t startOfBaseline, int32_t endOfBaseline, 
	uint16_t array[], const int32_t size, const uint16_t threshold, 
	const int32_t widthOfSequence, int32_t minPointsInBaseline, 
	int16_t *pBaseline, int16_t *pStdev);
int32_t calculateBaseline(int8_t *errorBoolean, char *errorMessage, 
	int32_t start, int32_t end, uint16_t array[],
	const int32_t size, const uint16_t threshold,
	int32_t minPointsInBaseline, int16_t *pBaseline,
	int16_t *pStdev);
bool inRange(const uint16_t threshold, double value);
void setErrorCluster(int8_t *errorBoolean, char *errorMessage,
	int16_t *pBaseline, int16_t *pStdev, const char *string);


_declspec (dllexport) int32_t findBaseline(int8_t *errorBoolean,
	char *errorMessage, int32_t startOfBaseline, int32_t endOfBaseline, 
	uint16_t array[], const int32_t size, const uint16_t threshold, 
	const int32_t widthOfSequence, int32_t minPointsInBaseline, 
	int16_t *pBaseline, int16_t *pStdev)
{
	// Error checking
	if (size < 0 || widthOfSequence < 0)
	{
		setErrorCluster(errorBoolean, errorMessage, pBaseline, pStdev, 
				   "Size of array and width should both be greater than zero");
		return (5000);
	}

	// Try to calculate the baseline starting from start. If it fails return
	// element of error i.e. starting point of error
	int32_t startOfError = calculateBaseline(errorBoolean, errorMessage, 
											 startOfBaseline, endOfBaseline,
											 array, size, threshold, 
											 minPointsInBaseline, pBaseline, 
											 pStdev);

	// If we find a baseline exit cleanly, else everything below -1 signifies 
	// error so return generated error. Because LABVIEW is not using the
	// positive >=5000 error codes we take the absolute value when returning
	if (startOfError == -1)
	{
		*errorBoolean = 0;
		const char *string = "No error";
		memcpy(errorMessage, string, strlen(string) + 1);
		return 0;
	}
	else if (startOfError < -1)
		return (abs(startOfError));

	// No baseline yet, so find next starting position, starting from
	// startOfError, by comparing sequences and returning if we find one
	// sequence and the next sequence is less smooth
	int32_t newStart = compareSequences(startOfError, (startOfError + 
										widthOfSequence), array, size, 
										widthOfSequence);

	// No next starting point...
	// So return with error value
	if (newStart == INT_MAX)
	{
		setErrorCluster(errorBoolean, errorMessage, pBaseline, pStdev,
						"No baseline found");
		return (5005);
	}

	// No problems so far, so update the end and try again
	int32_t newEnd = newStart + widthOfSequence;

	return (findBaseline(errorBoolean, errorMessage, newStart, newEnd, array, 
			size, threshold, widthOfSequence, minPointsInBaseline, pBaseline, 
			pStdev));
}

/*
 * This function tries to determine the baseline from a given array.
 * If we do not have sufficient points before we reach the pulse, the
 * function returns the element where it failed else if we do find
 * enough points, we set a pointer to the value of the calculated
 * baseline and return -1 because that can never be an element of an
 * array
 */
int32_t
calculateBaseline(int8_t *errorBoolean, char *errorMessage, int32_t start, 
					int32_t end, uint16_t array[],
					const int32_t size, const uint16_t threshold,
					int32_t minPointsInBaseline, int16_t *pBaseline, 
					int16_t *pStdev)
{
	// Declare variables
	int32_t i;
	double sum = 0;
	double average;
	double differenceFromAverage;
	double differenceInPoints;
	double minDifference;
	double maxDifference;

	// Make sure it is likely we have more than enough points to calculate the 
	// baseline
	if (start < 0 || end > size)
	{
		setErrorCluster(errorBoolean, errorMessage, pBaseline, pStdev,
						"Array out of bounds");
		return (-5001);
	}
	else if (start == end)
	{
		setErrorCluster(errorBoolean, errorMessage, pBaseline, pStdev,
				  "Starting point matches end point, so nothing to calculate");
		return (-5002);
	}
	else if ((end - start) < minPointsInBaseline)
	{
		setErrorCluster(errorBoolean, errorMessage, pBaseline, pStdev,
						"Array to small to calculate baseline");
		return (-5003);
	}

	// Set minimum and maximum of the whole, iterated over, part of the trace 
	// to account for the fact that we have a maximum deviation from the global
	// min and max and the program is not fooled by a very gradual in- / 
	// decreasimg trace.
	int min = array[start];
	int max = array[start];

	// Itereate over each element in the array
	// Start with second element (start + 1) because we need to compare it to a
	// previous element. We do not need more than 'end - start' elements to
	// accurately determine the baseline, the + 1 accounts for the fact that we
	// take the average until i - 1, which then translates to 'start' and 'end'
	for (i = (start + 1); i < (end + 1); i++)
	{
		// update minimum
		if (array[i] < min)
			min = array[i];

		// Update maximum
		if (array[i] > max)
			max = array[i];

		// Calculate the average up to i.e. not including the current element
		// Should go before threshold check, otherwise last element is not
		// included in baseline
		sum += array[i - 1];
		average = sum / (i - start);

		// Make sure min and max do not lay more than threshold away from the 
		// average. This could only be a minor issue if after a pulse or dip
		// the average in- or decrease so that the difference between the min/
		// max and the average becomes bigger than threshold. The script would
		// then break.
		minDifference = (double) average - min;
		maxDifference = (double) max - average;

		if (!inRange(threshold, minDifference)){
			i += 1;
			break;
		}
			
		if (!inRange(threshold, maxDifference)){
			i += 1;
			break;
		}

		// Determine difference between average and current point
		// Should go before difference between current point and previous point
		// because this is more likely to fail first
		differenceFromAverage = (double)array[i] - average;

		// Deviaton from average should fall within threshold
		if (!inRange(threshold, differenceFromAverage))
		{
			// Increment i with one so we can use -1 in the miliStdev
			// call, becasue we need to decrement i if we reach the end
			// of this for loop
			i += 1;
			break;
		}

		// Determine difference between current and previous point
		differenceInPoints = (double)array[i] - array[i - 1];

		// Make sure separate points are within the double threshold
		if (!inRange((threshold * 2), differenceInPoints))
		{
			// Increment i with one so we can use -1 in the miliStdev
			// call, becasue we need to decrement i if we reach the end
			// of this for loop
			i += 1;
			break;
		}
	}

	// We want al least 90% of bins iterated to be used for the 
	// baseline
	// If we have enough points to calculate the baseline set pointer to
	// baseline value and return (-1) else return element where calculating
	// failed.
	if ((i - start) >= (minPointsInBaseline * 0.9))
	{
		// Return -1 means everthing went ok and we've found a baseline 
		*pBaseline = (int16_t) round(average);

		// Make sure value of baseline not higher than around threshold + 
		// baseline
		if (*pBaseline > 220) {
			setErrorCluster(errorBoolean, errorMessage, pBaseline, pStdev,
							"Baseline to high");
			return (-5004);
		}

		// In order to comply with the old baseline filter return
		// stdev times 1000
		double miliStdev = stdev(start, (i - 1), array, average, size);
		*pStdev = (int16_t) round(miliStdev * 1000);

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

/*
 * Set the values for the error cluster, execpt for the errorValue, this is
 * just the return value for the function
 */
void
setErrorCluster(int8_t *errorBoolean, char *errorMessage, int16_t *pBaseline, 
	int16_t *pStdev, const char *string)
{
	*pBaseline = -999;
	*pStdev = -999;
	*errorBoolean = 1;
	memcpy(errorMessage, string, strlen(string) + 1);
}