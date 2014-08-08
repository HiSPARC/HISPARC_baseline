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
BASE = "C:\\Users\\SuperUser\\Documents\\HiSparc\\HISPARC_baseline\\test\\dll\\data\\"

# Save csv to file, folder should not start with slashes
def save_txt(detail, trace):
    FILE = os.path.join(BASE, str(detail) + ".csv")
    
    # Write trace to file
    with open(FILE, mode='w') as file:
        file.write(','.join(str(x) for x in trace))
    
# Set date of HDF5 file
DATE = "04-08-2014"

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

# Get current date and time
now = time.strftime("%H:%M:%S %d-%m-%Y")

# Open log file
fo = open("error_" + DATE + ".log", "w")
fo.write("#\n# This file contains the outcome of the comparison of the old and new implementation of the baseline filter\n#\n")
fo.write("# " + str(now) + "\n# Station 501 at " + DATE + "\n#\n\n")

fo.write("*" + 160 * "-" + "*\n")
fo.write("| Timestamp\t\tbaseline\tn_peaks\tn_peaks new\tpulseheights\tpulseheight new\tintegrals\tintegrals new\tleft cutoff \t right cutoff    |\n")
fo.write("*" + 160 * "-" + "*\n")

# Create empty dict
properties = {}

# Initiate progressbar
pbar = progressbar.ProgressBar(maxval=61133.,
                               widgets=[progressbar.Percentage(),
                                        progressbar.Bar(),
                                        progressbar.ETA()])

# Initialize counter
counter = 0

# Iterate over all events. Default is station 501 at Science Park
for x, (t, b, p, h, i, ti) in pbar(enumerate(get_traces_baseline())):

    if counter > 99:
        break
        
    # Loop over individual trace
    for y,(trace, bsl, n_peaks, pulseheight, integral, timestamp) in enumerate(zip(t, b, p, h, i, ti)):
        # Make sure we only get one trace per event
        if y > 0:
            continue
            
        # Define array
        trace_array = (c_uint16 * len(trace))(*trace)
        
        # Define function prototype as used in dll
        trace_Variables.argtypes = [c_uint16 * len(trace_array), c_int16, c_uint32, c_uint16, c_int16, POINTER(TVARIABLES)]
        trace_Variables.restype = c_int32

        # Create instance of variables class
        blah = TVARIABLES()  

        # Run the DLL
        dll_return = trace_Variables(trace_array, 26, len(trace), 25, bsl, byref(blah))
        
        # Check for all match and record 
        if n_peaks == blah.numberOfPeaks and n_peaks > 0 and pulseheight == blah.pulseHeight and integral == blah.pulseIntegral:
            counter += 1
            
            # Set ext_timestamp as key and create nested dictionary
            key = str(timestamp)
            properties[key] = {"n_peaks": int(n_peaks),
                               "pulseheights": int(pulseheight),
                               "integrals": int(integral),
                               "baseline": int(bsl),
                               "left_cutoff": int(blah.leftCutOff),
                               "right_cutoff": int(blah.rightCutOff)}
            
            save_txt(timestamp, trace)
            fo.write(str(timestamp) +"\t" + str(bsl) + "\t\t" + str(n_peaks) + "\t" + str(blah.numberOfPeaks) + "\t\t" + str(pulseheight) + "\t\t" + str(blah.pulseHeight) + "\t\t" + str(integral) + "\t\t" + str(blah.pulseIntegral) + "\t\t" + str(blah.leftCutOff) + "\t\t" + str(blah.rightCutOff) + "\n")

fo.close()

with open('data.json', mode='w') as f:
    json.dump(properties, f, indent=2) 
    
exit()
        
        