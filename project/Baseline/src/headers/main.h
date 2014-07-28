/*
 * ============================================================================
 *
 *       Filename:  main.h
 *
 *    Description:  Header file containing the main function declaration
 *					used to import the DLL using the shared library import
 *					option in LABVIEW.
 *
 *        Version:  1.0
 *        Created:  06-06-2014 11:18:21
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

#ifndef _MAIN_H
#define _MAIN_H
#define DECLDIR __declspec(dllexport)

// Set the number of point to include before and after the left / right cutoff
#define	EXTRAPOINTS 20

// Declare struct to hold trace properties
struct traceProperties
{
	int16_t numberOfPeaks;
	int16_t leftCutOff;
	int16_t rightCutOff;
	int16_t pulseHeight;
	int16_t pulseIntegral;
};

/*
 * Function to find the baseline, providing an array with a size. The initial
 * baseline is calculated from startInitBaseline to endInitBaseline, by using
 * the threshold to determine when an element is no longer part of the
 * baseline. If the calculation stops before minPointsInBaseline is reached,
 * the new startInitBaseline is determined using two sequences i.e. subarrays
 * of width: widthOFSequence. In the end the baseline and standard deviation
 * are returned.
 */
DECLDIR int32_t findBaseline(int32_t defaultStart, 
	int32_t baselineLength, uint16_t data_12_bit[], 
	const int32_t traceLength, const uint16_t baselineThreshold, 
	int32_t minimalInBaseline, int16_t *Baseline, int16_t *Stdev);

#endif