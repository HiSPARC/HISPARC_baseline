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

/*
 * Function to find the baseline, providing an array with a size. The initial
 * baseline is calculated from startInitBaseline to endInitBaseline, by using
 * the threshold to determine when an element is no longer part of the
 * baseline. If the calculation stops before minPointsInBaseline is reached,
 * the new startInitBaseline is determined using two sequences i.e. subarrays
 * of width: widthOFSequence. In the end the baseline and standard deviation
 * are returned.
 */
DECLDIR int32_t findBaseline(int32_t startInitBaseline, 
	int32_t endInitBaseline, uint16_t array[], 
	const int32_t size, const uint16_t threshold, 
	const int32_t widthOfSequence, int32_t minPointsInBaseline, 
	int16_t *Baseline, int16_t *Stdev);

#endif