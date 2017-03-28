Name of executable: mylocks

Special compiling instructions:
  1. run the following in command line: make

Example of running the program:
  -> make run
  -> make ARGS="-d 3" run               // run the 3rd test
  -> make ARGS="-i 100 -d 5" run        // run the 5th test with 100 iterations

Measurements:
-> See images folder

Things to Note:
- to run all tests including recursive test use:
  -> make ARGS="-i 4000 -d 10" run  // iteration is fixed to a small amount to avoid stack overflow
