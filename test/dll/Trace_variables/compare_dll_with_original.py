from __future__ import division
from ctypes import *
from sys import exit
import time
import os
import numpy as np
import progressbar

from get_trace_and_baseline import get_traces_baseline

# Set date of HDF5 file
DATE = "05-04-2014"

# Define the structure
class TVARIABLES(Structure):
    _fields_ = [("numberOfPeaks", c_int16), 
                ("leftCutOff", c_int16), 
                ("rightCutOff", c_int16),
                ("pulseHeight", c_int16),
                ("pulseIntegral", c_int32)]
                

# Make sure we can use the function from baseline.dll
baseline_dll = CDLL("../../../project/Baseline/Release/Baseline.dll")
trace_Variables = baseline_dll.traceVariables

# Initialise all counters
count_peaks = 0
count_pulse = 0
count_integral = 0
count_matches = 0
counter = 0

# Get current date and time
now = time.strftime("%H:%M:%S %d-%m-%Y")

# Open log file
fo = open("error_" + DATE + ".log", "w")
fo.write("#\n# This file contains the outcome of the comparison of the old and new implementation of the baseline filter\n#\n")
fo.write("# " + str(now) + "\n# Station 501 at " + DATE + "\n#\n\n")

fo.write("*-------------------------------------------------------------------------------------------------------------------*\n")
fo.write("| Timestamp\t\tbaseline\tn_peaks\tn_peaks new\tpulseheights\tpulseheight new\tintegrals\tintegrals new\tleft cutoff \t right cutoff    |\n")
fo.write("*-------------------------------------------------------------------------------------------------------------------*\n")

# Initiate progressbar
pbar = progressbar.ProgressBar(maxval=62484.,
                               widgets=[progressbar.Percentage(),
                                        progressbar.Bar(),
                                        progressbar.ETA()])

# Iterate over all events. Default is station 501 at Science Park
for x, (t, b, p, h, i, ti) in pbar(enumerate(get_traces_baseline())):

    # Loop over individual trace
    for trace, bsl, n_peaks, pulseheight, integral, timestamp in zip(t, b, p, h, i, ti):
        # Define array
        trace_array = (c_uint16 * len(trace))(*trace)
        
        # Define function prototype as used in dll
        trace_Variables.argtypes = [c_uint16 * len(trace_array), c_int16, c_uint32, c_uint16, c_int16, POINTER(TVARIABLES)]
        trace_Variables.restype = c_int32

        # Create instance of variables class
        blah = TVARIABLES()  

        # Run the DLL
        dll_return = trace_Variables(trace_array, 25, len(trace), 25, bsl, byref(blah))
        
        # Check for all possible mismatches and record them    
        if n_peaks != blah.numberOfPeaks:
            fo.write(str(timestamp) +"\t" + str(bsl) + "\t\t" + str(n_peaks) + "\t" + str(blah.numberOfPeaks) + "\t\t" + str(pulseheight) + "\t\t" + str(blah.pulseHeight) + "\t\t" + str(integral) + "\t\t" + str(blah.pulseIntegral) + "\t\t" + str(blah.leftCutOff) + "\t\t" + str(blah.rightCutOff) + "\n")
            count_peaks += 1
        elif pulseheight != blah.pulseHeight:
            fo.write(str(timestamp) +"\t" + str(bsl) + "\t\t" + str(n_peaks) + "\t" + str(blah.numberOfPeaks) + "\t\t" + str(pulseheight) + "\t\t" + str(blah.pulseHeight) + "\t\t" + str(integral) + "\t\t" + str(blah.pulseIntegral) + "\t\t" + str(blah.leftCutOff) + "\t\t" + str(blah.rightCutOff) + "\n")
            count_pulse += 1
        elif integral != blah.pulseIntegral:
            fo.write(str(timestamp) +"\t" + str(bsl) + "\t\t" + str(n_peaks) + "\t" + str(blah.numberOfPeaks) + "\t\t" + str(pulseheight) + "\t\t" + str(blah.pulseHeight) + "\t\t" + str(integral) + "\t\t" + str(blah.pulseIntegral) + "\t\t" + str(blah.leftCutOff) + "\t\t" + str(blah.rightCutOff) + "\n")
            count_integral += 1
        else:
            count_matches += 1
        
        counter += 1

# Calculate percentages
n_perc = (count_peaks / counter) * 100
p_perc = (count_pulse / counter) * 100
i_perc = (count_integral / counter) * 100
m_perc = (count_matches / counter) * 100

# Write stats and close file
fo.write("\n*-----------------------------------------*\n")        
fo.write("|            stats          |\n")
fo.write("*-----------------------------------------*\n")
fo.write("Total peak errors: " + str(count_peaks) + "\n")
fo.write("Total pulse errors: " + str(count_pulse) + "\n")
fo.write("Total integral errors: " + str(count_integral) + "\n")
fo.write("Total matches: " + str(count_matches) + "\n")
fo.write("Total traces looked at: " + str(counter) + "\n")
fo.write("Percentage of peak errors: " + str(n_perc) + "%\n")
fo.write("Percentage of pulse errors: " + str(p_perc) + "%\n")
fo.write("Percentage of integral errors: " + str(i_perc) + "%\n")
fo.write("Percentage of matches: " + str(m_perc) + "%\n")
fo.close()

exit()
        
        