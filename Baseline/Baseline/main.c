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

// Function declarations
_declspec (dllexport) int32_t determine_baseline(uint16_t adValues[], 
	const int32_t size, const int32_t threshold, double *baseline);

_declspec (dllexport) int32_t determine_baseline(uint16_t adValues[], 
	const int32_t size, const int32_t threshold, double *baseline)
{
	// Make sure it is likely we have enough points to calculate the baseline
	if (size < 50)
		return (5001);
}
