from __future__ import division
from ctypes import *
from sys import exit
import time
import os
import numpy as np
import progressbar
import json

from get_trace_and_baseline import get_traces_baseline

# Define base location to store all files
BASE = "C:\\Users\\SuperUser\\Documents\\HiSparc\\HISPARC_baseline\\test\\dll\\data_bsl\\"

# Save csv to file, folder should not start with slashes
def save_txt(detail, trace):
    FILE = os.path.join(BASE, str(detail) + ".csv")
    
    # Write trace to file
    with open(FILE, mode='w') as file:
        file.write(','.join(str(x) for x in trace))
        
# Set date of HDF5 file
DATE = "06-08-2014"

# Set types of the pointers to baseline and stdev
pBaseline = c_int16()
pStdev = c_int16()

# Make sure we can use the function from baseline.dll
baseline_dll = CDLL("../../../VI/dll/Baseline.dll")
findBaseline = baseline_dll.findBaseline

# Initialise all counters
counter = 0

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
pbar = progressbar.ProgressBar(maxval=62055.,
                               widgets=[progressbar.Percentage(),
                                        progressbar.Bar(),
                                        progressbar.ETA()])

# Create empty dict
properties = {}

# Iterate over all events. Default is station 501 at Science Park
for x, (t, b, s, ti) in pbar(enumerate(get_traces_baseline())):
    
    if counter > 99:
        break
        
    # Loop over individual trace
    for y, (trace, bsl, stdev, timestamp) in enumerate(zip(t, b, s, ti)):
    
        if y > 0:
            continue
            
        # Define array
        trace_array = (c_uint16 * len(trace))(*trace)
        
        # Define function prototype as used in dll
        findBaseline.argtypes = [c_int32, c_int32, c_uint16 * len(trace_array), c_int32, c_uint16, c_int32, POINTER(c_int16), POINTER(c_int16)]
        findBaseline.restype = c_int32
        
        # Run the DLL
        dll_return = findBaseline(0, len(trace), trace_array, len(trace), 25, 50, pBaseline, pStdev)
        
        # New and old matches so save this data for testing
        if pBaseline.value == bsl and pStdev.value == stdev:
            counter += 1
            
            # Set ext_timestamp as key and create nested dictionary
            key = str(timestamp)
            properties[key] = {"baseline": int(bsl),
                               "stdev": int(stdev)}
            
            save_txt(timestamp, trace)
            fo.write(str(timestamp) +"\t" + str(bsl) + "\t" + str(stdev) + "\n")
        
fo.close()

with open('data.json', mode='w') as f:
    json.dump(properties, f, indent=2)
    
exit()
        
        