from ctypes import *
from sys import exit

from get_trace_and_baseline import get_traces_baseline

# Set types of the pointers to baseline and stdev
pBaseline = c_int16()
pStdev = c_int16()

# Make sure we can use the function from baseline.dll
baseline_dll = CDLL("../../project/Baseline/Release/Baseline.dll")
findBaseline = baseline_dll.findBaseline

trace = [200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,226,230,235,240,239,234,231,228,226,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200]

# Define array
trace_array = (c_uint16 * len(trace))(*trace)
            
# Define function prototype as used in dll
findBaseline.argtypes = [c_int32, c_int32, c_uint16 * len(trace_array), c_int32, c_uint16, c_int32, POINTER(c_int16), POINTER(c_int16)]
findBaseline.restype = c_int32
            
# Run the DLL
dll_return = findBaseline(0, 40, trace_array, len(trace), 25, 40, pBaseline, pStdev)

print ("dll = ", dll_return)
print("baseline = ", pBaseline.value)
print("stdev = ", pStdev.value)