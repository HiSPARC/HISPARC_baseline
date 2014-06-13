from __future__ import division
from ctypes import *
from sys import exit
import time
import os
import numpy as np
import matplotlib.pyplot as plt


from get_trace_and_baseline import get_traces_baseline

# Save csv to file
def save_txt(timestr, trace):
	FILE = "data\\trace_" + timestr + ".csv"
	np.savetxt(FILE, trace, delimiter=",", fmt="%i")
	
# Set types of the pointers to baseline and stdev
pBaseline = c_int16()
pStdev = c_int16()

# Make sure we can use the function from baseline.dll
baseline_dll = CDLL("../../project/Baseline/Debug/Baseline.dll")
findBaseline = baseline_dll.findBaseline

# Initialise all counters
count_bsl_err = 0
count_stdev_err = 0
counter = 0

# Get current date and time
now = time.strftime("%H:%M:%S %d-%m-%Y")

# Open log file
fo = open("error_13-06-2014.log", "a")
fo.write("#\n# This file contains the outcome of the comparison of the old and new implementation of the baseline filter\n#\n")
fo.write("# " + str(now) + "\n#\n\n")
fo.write("*--------------------------------------------------------------------------------------------*\n")
fo.write("| Number\tbaseline_old\tbaseline_new\tstdev_old\tstdev_new\ttimestamp    |\n")
fo.write("*--------------------------------------------------------------------------------------------*\n")

# Iterate over all events. Default is station 501 at Science Park
for x, (t, b, s) in enumerate(get_traces_baseline()):
		
	for trace, bsl, stdev in zip(t, b, s):
		# Define array
		trace_array = (c_uint16 * len(trace))(*trace)
		
		# Define function prototype as used in dll
		findBaseline.argtypes = [c_int32, c_int32, c_uint16 * len(trace_array), c_int32, c_uint16, c_int32, POINTER(c_int16), POINTER(c_int16)]
		findBaseline.restype = c_int32
		
		dll_return = findBaseline(0, len(trace), trace_array, len(trace), 15, 100, pBaseline, pStdev)
		
		if pBaseline.value != bsl or  pStdev.value != stdev:
			print("-------------------", x ,"-----------------")
			
		if pBaseline.value != bsl:
			timestr = str(time.time())
			save_txt(timestr, trace)
			fo.write(str(x) +"\t\t" + str(bsl) + "\t\t" + str(pBaseline.value) + "\t\t" + str(stdev) + "\t\t" + str(pStdev.value) + "\t\t" + timestr + "\n")
			count_bsl_err += 1
		
		if pStdev.value != stdev:
			timestr = str(time.time())
			save_txt(timestr, trace)
			fo.write(str(x) +"\t\t" + str(bsl) + "\t\t" + str(pBaseline.value) + "\t\t" + str(stdev) + "\t\t" + str(pStdev.value) + "\t\t" + timestr + "\n")
			count_stdev_err += 1
		
		counter += 1


# Calculate percentages
b_perc = (count_bsl_err / counter) * 100
s_perc = (count_stdev_err / counter) * 100

# Write stats and close file
fo.write("\n\n*-----------------------------------------*\n")		
fo.write("|		    stats		  					  |\n")
fo.write("*-----------------------------------------*\n")
fo.write("Total baseline errors: " + str(count_bsl_err) + "\n")
fo.write("Total stdev errors: " + str(count_stdev_err) + "\n")
fo.write("Total traces looked at: " + str(counter) + "\n")
fo.write("Percentage of baseline errors: " + str(b_perc) + "%\n")
fo.write("Percentage of baseline errors: " + str(s_perc) + "%\n")
fo.close()

exit()
