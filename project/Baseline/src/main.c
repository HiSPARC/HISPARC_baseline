/* 
 * ============================================================================
 * 
 *			Filename:	main.c
 *		 Description:	This program calculates the average (baseline) of an
 *						array of value's up to the start of a significant
 *						pulse.
 *
 *			 Version:	1.17
 *			 Created:	06-06-2014 16:32:16
 *
 *			  Author:	Jorian van Oostenbrugge (jorianvo)
 *
 *			   Notes:	Call Library source file. Using labview compatible
 *						types (int32_t and uint16_t) and headers (extcode.h).
 *						In order to keep this file as clean as possible a few
 *						librabry files are used:
 *						- sequence.h/.c:	the header and source file which
 *											contain all code which is part
 *											of comparing sequences
 *						- main.h/.c:		the main, exported functions
 *						- intern.h/.c:		the header and source file which
 *											store function only used internally
 *											and thus are not exported
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
#include "intern.h"

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
 * This function calculates all trace properties. The trace variables consists
 * of the number of peaks, the left cutoff, the right cutoff, the pulseHeight
 * and the pulseIntegral. All properties are returned as a pointer to a struct
 * because labiew is unable to handle structs as a return object
 */
DECLDIR int32_t traceVariables(uint16_t data_12_bit[], int16_t PeakThreshold, 
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
		int16_t currentElement;
		properties->pulseHeight = 0;
		properties->pulseIntegral = 0;
		int16_t localMinimum = data_12_bit[0];
		int16_t localMaximum;

		// Is the current point bigger than the set threshold?
		bool overThreshold = false;

		// Is the left cutoff unset?
		bool leftUnset = true;

		// Is the right cutoff unset?
		bool rightUnset = true;

		// Is the slope rising?
		bool rising = false;

		// We have a baseline so calculate all trace properties
		for (int32_t i = 0; i < traceLength; i++)
		{
			// Current element relative to baseline
			currentElement = data_12_bit[i] - Baseline;

			// Get biggest value for the pulseheight
			if (currentElement > properties->pulseHeight)
				properties->pulseHeight = currentElement;

			// Determine total number of peaks, positive as well as negative
			// Start by determining the local minimum i.e. a value as close to
			// zero as possible
			if (abs(currentElement) < abs(localMinimum))
			{
				// We still need to know if this value if either positive or
				// negative
				localMinimum = currentElement;
			}
			else if (abs(currentElement - localMinimum) > PeakThreshold)
			{
				// Slope is rising in positive or negative direction
				rising = true;

				// The local maximum is now this point, so we are able to
				// find the top of a pulse
				localMaximum = currentElement;
			}

			// Found the start of a pulse (start above threshold)
			if (rising)
			{
				// Get top of pulse
				if (abs(currentElement) > abs(localMaximum))
					localMaximum = currentElement;
				else
				{
					// Found biggest point, so now see if we again go below
					// peak threshold (as measured from the top to make sure
					// every single peak is counted and not just a single
					// pulse)
					if (abs(localMaximum - currentElement) > PeakThreshold)
					{
						// That's a pulse!
						properties->numberOfPeaks += 1;

						// Reset start of pulse indicator
						rising = false;

						// Look again from here to a pulse
						localMinimum = currentElement;
					}
				}
			}

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

				// If this point belongs to a starting peak count it
				// (This point is not the starting point)
				if (rising)
					properties->numberOfPeaks += 1;
				else if (abs(currentElement) - abs(localMinimum) >
					PeakThreshold)
				{
					// Now at this point the peak start, so also count it
					properties->numberOfPeaks += 1;
				}
			}
		}
	}

	// Every property has a value so return
	return 0;
}