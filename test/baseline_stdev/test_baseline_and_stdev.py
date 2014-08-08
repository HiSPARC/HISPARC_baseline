import unittest
import os
from ctypes import *
import json


# The test data is from 06-08-2014 - station 501, the test assumes that the
# DLL works in the same way as the old implementation in LABVIEW, so changing
# any params in the self.find_baseline() function call may result in tests
# failing
BASELINE_THRESHOLD = 25
MINIMALINBASELINE = 50

TEST_DIR_NAME = "test_data"
TEST_DATA_DIR = os.path.join(os.path.dirname(__file__), TEST_DIR_NAME)

DLL_NAME = "Baseline.dll"
DLL_FILE = os.path.join(os.path.dirname(__file__), "../../VI/dll", DLL_NAME)

                
class TraceVariablesTests(unittest.TestCase):
    def setUp(self):
        # Load verified data as JSON object
        json_file = os.path.join(os.path.dirname(__file__), 'data.json')
        with open(json_file) as data:
            self.verified = json.load(data)
        
        # Import DLL and get traceVariables function
        dll = CDLL(DLL_FILE)
        self.find_baseline = dll.findBaseline
        
        # Set types of the pointers to baseline and stdev
        self.pBaseline = c_int16()
        self.pStdev = c_int16()

    def test_find_baseline(self):
        for key, value in self.verified.iteritems():
            # As key uniquely identifies a trace, get that trace
            trace = self.get_trace(key)
            
            # Set DLL (should be done every time because trace length varies)
            # Define array
            trace_array = (c_uint16 * len(trace))(*trace)
        
            # Define function prototype as used in DLL
            self.find_baseline.argtypes = [c_int32, c_int32, c_uint16 * len(trace_array), c_int32, c_uint16, c_int32, POINTER(c_int16), POINTER(c_int16)]
            self.find_baseline.restype = c_int32
            
            # To get correct baseline, use the JSON data
            bsl = self.verified[key]["baseline"]
            stdev = self.verified[key]["stdev"]
            
            # Run the DLL
            dll_return = self.find_baseline(0, len(trace), trace_array, len(trace), BASELINE_THRESHOLD, MINIMALINBASELINE, self.pBaseline, self.pStdev)
            
            self.assertEqual(self.pBaseline.value, bsl)
            self.assertEqual(self.pStdev.value, stdev)
    
    def get_trace(self, ext_timestamp):
        filepath = os.path.join(TEST_DATA_DIR, ext_timestamp + ".csv")
        with open(filepath, mode='r') as file:
            return [int(x) for x in file.read().split(',')]

  
if __name__ == '__main__':
    unittest.main()   