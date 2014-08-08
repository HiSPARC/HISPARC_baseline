from __future__ import division
from ctypes import *
from sys import exit
import time
import os
import numpy as np
import progressbar
import json

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
baseline_dll = CDLL("../../../VI/dll/Baseline.dll")
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
pbar = progressbar.ProgressBar(maxval=21805.,
                               widgets=[progressbar.Percentage(),
                                        progressbar.Bar(),
                                        progressbar.ETA()])

properties = {}

count = 0
     
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
        dll_return = trace_Variables(trace_array, 53, len(trace), 15, bsl, byref(blah))
        
        # Add cutoffs to respective list 
        left_list.append(blah.leftCutOff)
        right_list.append(blah.rightCutOff)
    
    # Find the maximum of the right list and the minimal of the left list
    data_length = (max(right_list) - min(left_list))
    trace_length = len(trace)
    
    # If the values do not match print all traces of the event to show whats
    # going on
    if not (data_length - 2 <= trace_length <= data_length + 2):
        fo.write("trace length = " + str(trace_length) + "\t\tminimal left cutoff is = " + str(min(left_list)) + "\t\tmaximum right list = " + str(max(right_list)) + "\t\t" + str(timestamp) + "\n")
        properties[str(timestamp)] = {"max right": int(max(right_list)),
                                     "max left": int(min(left_list))}
        count += 1
        
fo.close()

with open('data2.json', mode='w') as f:
    json.dump(properties, f, indent=2) 
    
exit()
        
        