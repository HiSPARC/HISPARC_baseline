from __future__ import division
from ctypes import *
from sys import exit
import time
import os
import numpy as np
import progressbar

from get_trace_and_baseline import get_traces_baseline
# Define base location to store all files
BASE = "C:\\Users\\SuperUser\\Documents\\HiSparc\\HISPARC_baseline\\test\\dll\\data\\"

# Save csv to file, folder should not start with slashes
def save_txt(detail, trace):
    FILE = os.path.join(BASE, "trace_" + detail + ".csv")
    np.savetxt(FILE, trace, delimiter=",", fmt="%i")
    
# Set date of HDF5 file
DATE = "22-07-2014"

# Set types of the pointers to baseline and stdev
pBaseline = c_int16()
pStdev = c_int16()

# Make sure we can use the function from baseline.dll
baseline_dll = CDLL("../../project/Baseline/Release/Baseline.dll")
findBaseline = baseline_dll.findBaseline

# Initialise all counters
count_matches = 0

# Get current date and time
now = time.strftime("%H:%M:%S %d-%m-%Y")

# Open log file
fo = open("error_" + DATE + ".log", "w")
fo.write("#\n# This file contains the outcome of the comparison of the old and new implementation of the baseline filter\n#\n")
fo.write("# " + str(now) + "\n# Station 501 at " + DATE + "\n#\n\n")

fo.write("*-----------------------------------------------------------------------------------------------------------------------------------*\n")
fo.write("| Number\tbaseline_old\tbaseline_new\tstdev_old\tstdev_new\ttimestamp\t\t\terrorValue    |\n")
fo.write("*-----------------------------------------------------------------------------------------------------------------------------------*\n")

# Initiate progressbar
pbar = progressbar.ProgressBar(maxval=60287.,
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
		findBaseline.argtypes = [c_int32, c_int32, c_uint16 * len(trace_array), c_int32, c_uint16, c_int32, POINTER(c_int16), POINTER(c_int16)]
		findBaseline.restype = c_int32
		
		# Run the DLL
		dll_return = findBaseline(0, len(trace), trace_array, len(trace), 25, 50, pBaseline, pStdev)
		
		# Check for all possible mismatches and record them
		if pBaseline.value == bsl and pStdev.value == stdev:
			count_matches += 1
			
			if count_matches > 50:
			    break
			
			timestr = str(time.time())
			fo.write(timestr +"\t\t" + str(bsl) + "\t\t" + str(pBaseline.value) + "\t\t" + str(stdev) + "\t\t" + str(pStdev.value) + "\t\t" + str(timestamp) + "\t\t" + str(dll_return) + "\t\t" + "\n")
			save_txt(timestr, trace)
		
pbar.finish()
fo.close()

exit()
		
		