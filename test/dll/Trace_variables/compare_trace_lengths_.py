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
count_wrong_cutoff = 0
count_matches = 0
counter = 0

# Get current date and time
now = time.strftime("%H:%M:%S %d-%m-%Y")

# Open log file
fo = open("error_" + DATE + ".log", "w")
fo.write("#\n# This file contains the outcome of the comparison of the old and new implementation of the baseline filter\n#\n")
fo.write("# " + str(now) + "\n# Station 501 at " + DATE + "\n#\n\n")

fo.write("*" + 160 * "-" + "*\n")
fo.write("| Timestamp\t\tbaseline\tn_peaks\tn_peaks new\tpulseheights\tpulseheight new\tintegrals\tintegrals new\tleft cutoff \t right cutoff    |\n")
fo.write("*" + 160 * "-" + "*\n\n")

# Initiate progressbar
pbar = progressbar.ProgressBar(maxval=62484.,
                               widgets=[progressbar.Percentage(),
                                        progressbar.Bar(),
                                        progressbar.ETA()])

# Iterate over all events. Default is station 501 at Science Park
for x, (t, b, p, h, i, ti) in pbar(enumerate(get_traces_baseline())): 
    
    # Create empty lists to store all calculated left and right cutoffs
    left_list = []
    right_list = []
    
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
        dll_return = trace_Variables(trace_array, 52, len(trace), 25, bsl, byref(blah))
        
        # Add cutoffs  to respective list 
        left_list.append(blah.leftCutOff)
        right_list.append(blah.rightCutOff)
    
    # Find the maximum of the right list and the minimal of the left list
    data_length = (max(right_list) - min(left_list))
    trace_length = len(trace)
    
    # If the values do not match print all traces of the event to show whats
    # going on
    if not (data_length - 2 <= trace_length <= data_length + 2):
        count_wrong_cutoff += 1
        
        fo.write("*" + 160 * "-" + "*\n")
        fo.write("trace length = " + str(trace_length) + ", minimal left cutoff is = " + str(min(left_list)) + ", maximum right list = " + str(max(right_list)) + "\n")
        fo.write("*" + 160 * "-" + "*\n")
        for left, right in zip(left_list, right_list):
            fo.write(str(timestamp) +"\t" + str(bsl) + "\t\t" + str(n_peaks) + "\t" + str(blah.numberOfPeaks) + "\t\t" + str(pulseheight) + "\t\t" + str(blah.pulseHeight) + "\t\t" + str(integral) + "\t\t" + str(blah.pulseIntegral) + "\t\t" + str(left) + "\t\t" + str(right) + "\t\t" + str(trace_length) + "\n")
    else:
        count_matches += 1
            
    counter += 1
        
# Calculate percentages
n_perc = (count_wrong_cutoff / counter) * 100
m_perc = (count_matches / counter) * 100

# Write stats and close file
fo.write("\n*-----------------------------------------*\n")        
fo.write("|            stats          |\n")
fo.write("*-----------------------------------------*\n")
fo.write("Total cutoff errors: " + str(count_wrong_cutoff) + "\n")
fo.write("Total matches: " + str(count_matches) + "\n")
fo.write("Total traces looked at: " + str(counter) + "\n")
fo.write("Percentage of cutoff errors: " + str(n_perc) + "%\n")
fo.write("Percentage of matches: " + str(m_perc) + "%\n")
fo.close()

exit()
        
        