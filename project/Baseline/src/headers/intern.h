/*
 * ============================================================================
 *
 *       Filename:  intern.h
 *
 *    Description:  Header file containing function only used internally in the
 *					DLL and thus not need to be exported
 *
 *        Version:  1.0
 *        Created:  29-07-2014 14:25:22
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
#include <stdbool.h>

#ifndef _INTERN_H
#define _INTERN_H

/*
* This function tries to determine the baseline from a given array.
* If we do not have sufficient points before we reach the pulse, the
* function returns the element where it failed else if we do find
* enough points, we set a pointer to the value of the calculated
* baseline and return -1 because that can never be an element of an
* array. The start indicates the start of the baseline and the end denotes
* the last element included in the baseline - if there is no pulse ofcourse.
*/
int32_t calculateBaseline(int32_t start, int32_t end, uint16_t array[],
	const int32_t size, const uint16_t threshold,
	int32_t minimalInBaseline, int16_t *Baseline, int16_t *Stdev);

/*
* Check if a value lies within the absolute value of the supplied threshold
* i.e. more than the negative value of the threshold and less than the
* positive value of the threshold
*/
bool inRange(const uint16_t threshold, double value);

/*
* This function is called on the occurrence of an error to set the
* baseline and standard deviation accordingly
*/
void setErrorValues(int16_t *Baseline, int16_t *Stdev);

#endif