# dram-tester

This is a DRAM test code, which I used to test my video card chips in that
video: https://www.youtube.com/watch?v=MZvCcadwblg&t=330s

Arduino seems to be too slow to keep up with required RAS-to-CAS delay. I got it
as fast as I could so far and it seems to be sufficient to make some simple
read/write tests. However, I couldn't get managed to write a complete run
through tests, writing/reading patterns and validating all address space. The
program seem to run for some time, but suddenly stops working after a while.
The patterns do not match, despite, that I know, that the chip is ok. I tested
it on many chips with different timings, but unfortunately all with the same
result. I guess, it's due to some timing issues and because Arduino is just not
able to keep up with required delays. Anyway, this code is sufficient to make
some random tests, this doesn't tell if the whole DRAM address space is good,
but at least it is sufficient to tell if the memory chips are working at all.

