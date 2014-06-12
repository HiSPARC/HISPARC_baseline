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
- [ ] Use matplotlib and python to check current implementation against the
      new and improved version. Maybe loop over 300 traces and compare found
      known baseline and stdev against the numbers returned by my
      implementation.
