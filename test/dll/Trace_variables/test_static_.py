from ctypes import *
from sys import exit

# Define the structure
class TVARIABLES(Structure):
    _fields_ = [("numberOfPeaks", c_int16), 
                ("leftCutOff", c_int16), 
                ("rightCutOff", c_int16),
                ("pulseHeight", c_int16),
                ("pulseIntegral", c_int16)]

# Create instance of variables class
blah = TVARIABLES()              
              
# Make sure we can use the function from baseline.dll
baseline_dll = CDLL("../../../project/Baseline/Release/Baseline.dll")
trace_Variables = baseline_dll.traceVariables

trace = [200,200,226,235,250,240,225,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,26,20,15,10,8,12,16,21,26,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200]

# Define array
trace_array = (c_uint16 * len(trace))(*trace)
            
# Define function prototype as used in dll
trace_Variables.argtypes = [c_uint16 * len(trace_array), c_int16, c_uint32, c_uint16, c_int16, POINTER(TVARIABLES)]
trace_Variables.restype = c_int32
            
# Run the DLL
dll_return = trace_Variables(trace_array, 25, len(trace), 25, 200, byref(blah))

# Show the output
print ("dll = ", dll_return)
print("number of peaks = ", blah.numberOfPeaks)