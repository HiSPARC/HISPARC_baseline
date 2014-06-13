from ctypes import *
from sys import exit
import time
import os
import numpy as np
import matplotlib.pyplot as plt

from get_trace_and_baseline import get_traces_baseline

def save_txt(timestr, trace):
	FILE = "trace_" + timestr + ".csv"
	np.savetxt(FILE, trace, delimiter=",")
	
# Set types of the pointers to baseline and stdev
pBaseline = c_int16()
pStdev = c_int16()

# Make sure we can use the function from baseline.dll
baseline_dll = CDLL("../../project/Baseline/Debug/Baseline.dll")
findBaseline = baseline_dll.findBaseline

count_bsl_err = 0
count_stdev_err = 0
counter = 0

# Iterate over all events. Default is station 501 at Science Park
for x, (t, b, s) in enumerate(get_traces_baseline()):

	if x == 10:
		break
		
	for trace, bsl, stdev in zip(t, b, s):
		# Define array
		trace_array = (c_uint16 * len(trace))(*trace)
		
		# Define function prototype as used in dll
		findBaseline.argtypes = [c_int32, c_int32, c_uint16 * len(trace_array), c_int32, c_uint16, c_int32, POINTER(c_int16), POINTER(c_int16)]
		findBaseline.restype = c_int32
		
		dll_return = findBaseline(0, len(trace), trace_array, len(trace), 15, 100, pBaseline, pStdev)
		
		if pBaseline.value != bsl or  pStdev.value != stdev:
			print("-------------------Next-----------------")
			
		if pBaseline.value != bsl:
			timestr = str(time.time())
			save_txt(timestr, trace)
			plt.figure()
			plt.plot(trace)
			plt.ylim([100,900])
			plt.title(timestr)
			print bsl
			print pBaseline.value
			count_bsl_err += 1
		
		if pStdev.value != stdev:
			timestr = str(time.time())
			save_txt(timestr, trace)
			plt.figure()
			plt.plot(trace)
			plt.ylim([100,900])
			plt.title(timestr)
			print stdev
			print pStdev.value
			count_stdev_err += 1
		
		counter += 1
		
plt.show()	
print ("Total traces = ", counter)
print ("Total mismatches bsl = ", count_bsl_err)
print ("Total mismatches stdev = ", count_stdev_err)

exit()
