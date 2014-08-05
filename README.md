Determining the baseline
========================

This filter should try and determine the baseline for any given signal
----------------------------------------------------------------------

- [x] Create repository to contain all files
- [x] Add testing module to allow for unit-testing use MinUnit
- [x] Create separte library for function to allow the unit-testing
- [x] Create function to determine the baseline just starting at zero
- [x] Create some (recursive?) function/statements to allow for
      compareSequences to run if determineBaseline can't find enough points.
- [x] Stdev in Calculate_Baseline times 1000, why?
- [x] What to do with stdev if baseline returns error?
- [x] Use matplotlib and python to check current implementation against the
      new and improved version. Maybe loop over 300 traces and compare found
      known baseline and stdev against the numbers returned by my
      implementation.
- [x] Tested and ready to be implemented.


The implementation
------------------

- [x] Update the DLL to statically link to the msvc.....dll library to be
	  independent of any MSVC packages (updated Release build but not tested it
	  yet).
- [x] Create subVI from `library function node` to include documentation from within LABVIEW
- [x] Create Trace Variables vi which calculates all trace variables
- [ ] Build python test module to store all tests in one place and allow for easy testing
- [ ] Remove src folder from this repo because src and Dll have diverged to much
- [ ] Create gh-pages documentation describing how the DLL works and how it is maintained
