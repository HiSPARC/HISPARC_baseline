import unittest
import os
from ctypes import *
import json


# The test data is from 04-08-2014, and the peak threshold was set to 26
PEAK_THRESHOLD = 26

TEST_DIR_NAME = "test_data"
TEST_DATA_DIR = os.path.join(os.path.dirname(__file__), TEST_DIR_NAME)

DLL_NAME = "Baseline.dll"
DLL_FILE = os.path.join(os.path.dirname(__file__), "../../VI/dll", DLL_NAME)


# Define structure which hold trace variables
class TVARIABLES(Structure):
    _fields_ = [("numberOfPeaks", c_int16), 
                ("leftCutOff", c_int16), 
                ("rightCutOff", c_int16),
                ("pulseHeight", c_int16),
                ("pulseIntegral", c_int32)]
                
class TraceVariablesTests(unittest.TestCase):
    def setUp(self):
        # Load verified data as JSON object
        with open('data.json') as data:
            self.verified = json.load(data)
        
        # Import DLL and get traceVariables function
        dll = CDLL(DLL_FILE)
        self.trace_variables = dll.traceVariables

    def test_trace_variables(self):
        for key, value in self.verified.iteritems():
            # As key uniquely identifies a trace, get that trace
            trace = self.get_trace(key)
            
            # Set DLL (should be done every time because trace length varies)
            # Define array
            trace_array = (c_uint16 * len(trace))(*trace)
        
            # Define function prototype as used in DLL
            self.trace_variables.argtypes = [c_uint16 * len(trace_array), c_int16, c_uint32, c_uint16, c_int16, POINTER(TVARIABLES)]
            self.trace_variables.restype = c_int32
            
            # Create variables object
            calculated_variables = TVARIABLES()
            
            # To get correct baseline, use the JSON data
            bsl = self.verified[key]["baseline"]
            n_peaks = self.verified[key]["n_peaks"]
            pulseheight = self.verified[key]["pulseheights"]
            integral = self.verified[key]["integrals"]
            
            # Run the DLL
            dll_run = self.trace_variables(trace_array, PEAK_THRESHOLD, len(trace), 25, bsl, byref(calculated_variables))
            
            self.assertEqual(calculated_variables.numberOfPeaks, n_peaks)
            self.assertEqual(calculated_variables.pulseHeight, pulseheight)
            self.assertEqual(calculated_variables.pulseIntegral, integral)
    
    def get_trace(self, ext_timestamp):
        filepath = os.path.join(TEST_DATA_DIR, ext_timestamp + ".csv")
        with open(filepath, mode='r') as file:
            return [int(x) for x in file.read().split(',')]

  
if __name__ == '__main__':
    unittest.main()   