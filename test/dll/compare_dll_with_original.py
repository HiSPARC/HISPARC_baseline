from __future__ import division
from ctypes import *
from sys import exit
import time
import os
import numpy as np
import progressbar

from get_trace_and_baseline import get_traces_baseline

# Set date of HDF5 file
DATE = "18-06-2014"

# Set types of the pointers to baseline and stdev
pBaseline = c_int16()
pStdev = c_int16()

# Make sure we can use the function from baseline.dll
baseline_dll = CDLL("../../project/Baseline/Release/Baseline.dll")
findBaseline = baseline_dll.findBaseline

# Initialise all counters
count_bsl_err = 0
count_stdev_err = 0
count_bsl_stdev_err = 0
count_matches = 0
counter = 0

# Get current date and time
now = time.strftime("%H:%M:%S %d-%m-%Y")

# Open log file
fo = open("error_" + DATE + ".log", "w")
fo.write("#\n# This file contains the outcome of the comparison of the old and new implementation of the baseline filter\n#\n")
fo.write("# " + str(now) + "\n# Station 501 at " + DATE + "\n#\n\n")

fo.write("*---------------------------------------------------------------------------------------------------------------------------------------------------------------*\n")
fo.write("| Number\tbaseline_old\tbaseline_new\tstdev_old\tstdev_new\ttimestamp\t\t\terrorValue\terrorBoolean\terrorMessage    |\n")
fo.write("*---------------------------------------------------------------------------------------------------------------------------------------------------------------*\n")

# Initiate progressbar
pbar = progressbar.ProgressBar(maxval=58704.,
							   widgets=[progressbar.Percentage(),
										progressbar.Bar(),
										progressbar.ETA()]).start()

# Iterate over all events. Default is station 501 at Science Park
for x, (t, b, s, ti) in pbar(enumerate(get_traces_baseline())):
	
	# Loop over individual trace
	for trace, bsl, stdev, timestamp in zip(t, b, s, ti):
		# Define array
		trace_array = (c_uint16 * len(trace))(*trace)
		
		# Define function prototype as used in dll
		findBaseline.argtypes = [c_int32, c_int32, c_uint16 * len(trace_array), c_int32, c_uint16, c_int32, c_int32, POINTER(c_int16), POINTER(c_int16)]
		findBaseline.restype = c_int32
		
		# Run the DLL
		dll_return = findBaseline(0, len(trace), trace_array, len(trace), 25, 100, 50, pBaseline, pStdev)
		
		# Check for all possible mismatches and record them	
		if pBaseline.value != bsl and pStdev.value == stdev:
			fo.write(str(x) +"\t\t" + str(bsl) + "\t\t" + str(pBaseline.value) + "\t\t" + str(stdev) + "\t\t" + str(pStdev.value) + "\t\t" + str(timestamp) + "\t\t" + str(dll_return) + "\t\t" + "\n")
			count_bsl_err += 1
		elif pBaseline.value != bsl and pStdev.value != stdev:
			fo.write(str(x) +"\t\t" + str(bsl) + "\t\t" + str(pBaseline.value) + "\t\t" + str(stdev) + "\t\t" + str(pStdev.value) + "\t\t" + str(timestamp) + "\t\t" + str(dll_return) + "\t\t" + "\n")
			count_bsl_stdev_err += 1
		elif pBaseline.value == bsl and pStdev.value != stdev:
			fo.write(str(x) +"\t\t" + str(bsl) + "\t\t" + str(pBaseline.value) + "\t\t" + str(stdev) + "\t\t" + str(pStdev.value) + "\t\t" + str(timestamp) + "\t\t" + str(dll_return) + "\t\t" + "\n")
			count_stdev_err += 1
		else:
			count_matches += 1
		
		counter += 1
		
pbar.finish()

# Calculate percentages
b_perc = (count_bsl_err / counter) * 100
s_perc = (count_stdev_err / counter) * 100
b_s_perc = (count_bsl_stdev_err / counter) * 100
m_perc = (count_matches / counter) * 100

# Write stats and close file
fo.write("\n*-----------------------------------------*\n")		
fo.write("|		    stats		  |\n")
fo.write("*-----------------------------------------*\n")
fo.write("Total baseline only errors: " + str(count_bsl_err) + "\n")
fo.write("Total stdev only errors: " + str(count_stdev_err) + "\n")
fo.write("Total baseline and stdev errors: " + str(count_bsl_stdev_err) + "\n")
fo.write("Total matches: " + str(count_matches) + "\n")
fo.write("Total traces looked at: " + str(counter) + "\n")
fo.write("Percentage of baseline only errors: " + str(b_perc) + "%\n")
fo.write("Percentage of stdev only errors: " + str(s_perc) + "%\n")
fo.write("Percentage of baseline and stdev errors: " + str(b_s_perc) + "%\n")
fo.write("Percentage of matches: " + str(m_perc) + "%\n")
fo.close()

exit()
		
		