/* 
 * ============================================================================
 * 
 *			Filename:	main.c
 *		 Description:	This program calculates the average (baseline) of an
 *						array of value's up to the start of a significant
 *						pulse.
 *
 *			 Version:	1.15
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
#include "main.h"

// Function declarations
int32_t calculateBaseline(int32_t start, int32_t end, uint16_t array[],
	const int32_t size, const uint16_t threshold, int32_t minimalInBaseline, 
	int16_t *Baseline, int16_t *Stdev);
bool inRange(const uint16_t threshold, double value);
void setErrorValues(int16_t *Baseline, int16_t *Stdev);

// The main (exported) function
DECLDIR int32_t findBaseline(int32_t defaultStart, int32_t baselineLength, 
	uint16_t data_12_bit[], const int32_t traceLength, 
	const uint16_t baselineThreshold, int32_t minimalInBaseline, 
	int16_t *Baseline, int16_t *Stdev)
{
	// No event data present
	if (traceLength <= 0)
	{
		setErrorValues(Baseline, Stdev);
		return (5000);
	}	

	// Try to calculate the baseline starting from start. If it fails return
	// element of error i.e. starting point of error
	int32_t startOfError = calculateBaseline(defaultStart, 
											 baselineLength, data_12_bit, 
											 traceLength, baselineThreshold, 
											 minimalInBaseline, Baseline, 
											 Stdev);
	
	// If we find a baseline exit cleanly, else everything below -1 signifies 
	// error so return generated error. Because LABVIEW is not using the
	// positive >=5000 error codes we take the absolute value when returning
	if (startOfError == -1)
		return 0;
	else if (startOfError < -1)
		return (abs(startOfError));

	// Since startOfError will most likely return the start of an pulse, we
	// first update the start for comparing sequences by width of sequence
	// to jump over the error instead of calculating all properties which takes
	// time
	int32_t startComparing = startOfError + SEQUENCEWIDTH;
	int32_t endFirstSequence = startComparing + SEQUENCEWIDTH;

	// No baseline yet, so find next starting position, starting from
	// startOfError, by comparing sequences and returning if we find one
	// sequence and the next sequence is less smooth
	int32_t newStart = compareSequences(startComparing, endFirstSequence, 
										data_12_bit, traceLength,
										SEQUENCEWIDTH);
	
	// No next starting point...
	// So return with error value
	if (newStart == INT_MAX)
	{
		setErrorValues(Baseline, Stdev);
		return (5005);
	}

	// No problems so far, so update the end and try again
	int32_t newEnd = newStart + minimalInBaseline;

	return (findBaseline(newStart, newEnd, data_12_bit,
		traceLength, baselineThreshold, minimalInBaseline, Baseline, Stdev));
}

/*
 * This function tries to determine the baseline from a given array.
 * If we do not have sufficient points before we reach the pulse, the
 * function returns the element where it failed else if we do find
 * enough points, we set a pointer to the value of the calculated
 * baseline and return -1 because that can never be an element of an
 * array. The start indicates the start of the baseline and the end denotes
 * the last element included in the baseline - if there is no pulse ofcourse.
 */
int32_t
calculateBaseline(int32_t start, int32_t end, uint16_t array[],
				  const int32_t size, const uint16_t threshold, 
				  int32_t minimalInBaseline, int16_t *Baseline, int16_t *Stdev)
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
		// Array out of bounds
		setErrorValues(Baseline, Stdev);
		return (-5001);
	}
	else if (start == end)
	{
		// Nothing to calculate
		setErrorValues(Baseline, Stdev);
		return (-5002);
	}
	else if ((end - start) < minimalInBaseline)
	{
		// Not enough points
		setErrorValues(Baseline, Stdev);
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

	// If we have enough points to calculate the baseline set pointer to
	// baseline value and return (-1) else return element where calculating
	// failed.
	if (((i - 1) - start) >= minimalInBaseline)
	{
		// Return -1 means everthing went ok and we've found a baseline 
		*Baseline = (int16_t) round(average);

		// Make sure value of baseline not higher than around threshold + 
		// baseline
		if (*Baseline > 220)
		{
			setErrorValues(Baseline, Stdev);
			return (-5004);
		}

		// In order to comply with the old baseline filter return
		// stdev times 1000
		double miliStdev = stdev(start, (i - 1), array, average, size);
		*Stdev = (int16_t) round(miliStdev * 1000);

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
 * This function is called on the occurrence of an error to set the
 * baseline and standard deviation accordingly
 */
void
setErrorValues(int16_t *Baseline, int16_t *Stdev)
{
	// The -999 indicates the occurrence of an error
	*Baseline = -999;
	*Stdev = -999;
}

/*
 * This function calculates all trace variables
 */
int32_t
traceVariables(uint16_t data_12_bit[], int16_t PeakThreshold, 
			   const int32_t traceLength, const uint16_t baselineThreshold, 
			   const int16_t Baseline, struct traceProperties *properties)
{
	// In order to calculate the trace variables we need a baseline
	// so if we have none set all derived properties to -999
	if (Baseline == -999)
	{
		// No derived properties
		properties->numberOfPeaks = -999;
		properties->pulseHeight = -999;
		properties->pulseIntegral = -999;

		// Send the whole trace
		properties->leftCutOff = 0;
		properties->rightCutOff = traceLength;
	}
	else
	{
		// Declare variables
		uint16_t currentElement;
		properties->pulseHeight = 0;
		properties->pulseIntegral = 0;

		// Is the current point bigger than the set threshold?
		bool overThreshold = false;

		// Is the left cutoff unset?
		bool leftUnset = true;

		// Is the right cutoff unset?
		bool rightUnset = true;

		// We have a baseline so calculate all trace properties
		for (int32_t i = 0; i < traceLength; i++)
		{
			// Current element relative to baseline
			currentElement = data_12_bit[i] - Baseline;

			// Get biggest value for the pulseheight
			if (currentElement > properties->pulseHeight)
				properties->pulseHeight = currentElement;

			// Compare current element to threshold to determine integral
			// and cutoffs
			if (currentElement > baselineThreshold)
			{
				// Be ready for first point lower than threshold
				overThreshold = true;

				// Current point is part of pulseIntegral so add it
				properties->pulseIntegral += currentElement;

				// If it is the first point to cross threshold boundry
				// it is also the left cutoff
				if (leftUnset)
				{
					// Set left cutoff to current element but with 20 extra
					// elements to be able to calculate the baseline after
					// data reduction. Thus make sure we have room for those
					// extra elements
					if (i <= EXTRAPOINTS)
						properties->leftCutOff = 0;
					else
						properties->leftCutOff = (i - EXTRAPOINTS);

					// This is the left cutoff so never update this value again
					leftUnset = false;
				}
			}
			else
			{
				// This point is the first point after a pulse so potentially
				// right cutoff
				if (overThreshold)
				{
					// Set right cutoff to current element but with 20 extra
					// elements to be able to calculate the baseline after
					// data reduction. Thus make sure we have room for those
					// extra elements
					if ((i + EXTRAPOINTS) >= traceLength)
						properties->rightCutOff = traceLength;
					else
						properties->rightCutOff = (i + EXTRAPOINTS);

					// Return to false so at the next pulse we are triggered
					// again but not before that pulse
					overThreshold = false;

					// Right cutoff is now set
					rightUnset = false;
				}
			}

			// Reached the last point but no pulse found so set the left
			// cutoff point to the rightmost value i.e. trace length and 
			// the right cutoff point to the leftmost value i.e. 0 (zero) 
			// to make sure that these points are never chosen
			if (i == (traceLength - 1))
			{
				if (leftUnset)
					properties->leftCutOff = traceLength;

				if (rightUnset)
					properties->rightCutOff = 0;

				// As the last point of the trace array sits within a pulse
				// we send the whole trace for analysis
				if (overThreshold)
					properties->rightCutOff = i;
			}
		}

		// Every property is calculated so exit
		return 0;
	}
}