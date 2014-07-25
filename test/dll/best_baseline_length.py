from __future__ import division
from ctypes import *
from sys import exit
import time
import os
import numpy as np
import progressbar

from get_trace_and_baseline import get_traces_baseline

# Set date of HDF5 file
DATE = "22-07-2014"

# Set types of the pointers to baseline and stdev
pBaseline = c_int16()
pStdev = c_int16()

# Make sure we can use the function from baseline.dll
baseline_dll = CDLL("../../project/Baseline/Release/Baseline.dll")
findBaseline = baseline_dll.findBaseline

# Get current date and time
now = time.strftime("%H:%M:%S %d-%m-%Y")

# Open log file
fo = open("error_" + DATE + ".log", "w")
fo.write("#\n# This file contains the connection between the length of the baseline and the value of the standard deviation\n#\n")
fo.write("# " + str(now) + "\n# Station 501 at " + DATE + "\n#\n\n")

fo.write("*-----------------------------------------------------------------------------------------------------------*\n")
fo.write("| Length of Baseline\t\tAverage stdev\t\tNumber of traces\t\tAverage Baseline    |\n")
fo.write("*-----------------------------------------------------------------------------------------------------------*\n")

# Initiate progressbar
pbar = progressbar.ProgressBar(maxval=4.,
                               widgets=[progressbar.Percentage(),
                                        progressbar.Bar(),
                                        progressbar.ETA()]).start()

# Use a variable trace length
for base_length in pbar(range(30, 1005, 5)):

    # Initialise all counters
    count_matches = 0
    sum_stdev = 0
    sum_bsl = 0

    # Iterate over all events. Default is station 501 at Science Park
    for x, (t, b, s, ti) in enumerate(get_traces_baseline()):
    
        if x > 400:
            break
        
        # Loop over individual trace
        for trace, bsl, stdev, timestamp in zip(t, b, s, ti):
            # Define array
            trace_array = (c_uint16 * len(trace))(*trace)
            
            # Define function prototype as used in dll
            findBaseline.argtypes = [c_int32, c_int32, c_uint16 * len(trace_array), c_int32, c_uint16, c_int32, POINTER(c_int16), POINTER(c_int16)]
            findBaseline.restype = c_int32
            
            
            # Run the DLL
            dll_return = findBaseline(0, base_length, trace_array, len(trace), 25, base_length, pBaseline, pStdev)
            
            # We don't care about a match as long as the values are not an error value
            # If a match occurs count the match and update the total stdev sum 
            if pBaseline.value != -999 and pStdev.value != -999 and pStdev.value != 32767:
                count_matches += 1
                sum_stdev += pStdev.value
                sum_bsl += pBaseline.value
    
    # One trace done calculate the averages
    average_stdev = sum_stdev / count_matches
    average_bsl = sum_bsl / count_matches
    
    # Write to log file
    fo.write(str(base_length) + "\t\t\t\t" + str(average_stdev) + "\t\t" + str(count_matches) + "\t\t\t\t" + str(average_bsl) + "\n")
            
        
pbar.finish()
fo.close()

exit()
        
        