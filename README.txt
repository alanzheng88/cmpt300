Name of Executable: ./assn2
Compilation Instructions:
  1. make
  2. make run
Note:
  The results below are in nanoseconds. The first number (e.g: 366 of minimal function call) is the result of the first execution of the loop. The second number (e.g: 96 of minimal function call) is the result of the second execution of the loop.. etc
  Also, I tried getting threads to work but timer isn't working.


366, 96, 90, 95, 95, 90, 90, 85, 90, 86, 95, 91, 90, 91, 90, 85, 90, 85, 95, 90
cost of minimal function call: 104ns

1544, 240, 116, 100, 100, 100, 100, 95, 100, 95, 100, 105, 105, 100, 100, 105, 100, 100, 105, 100
cost of minimal system call: 180ns

128779, 28195, 42914, 30926, 24417, 21158, 17205, 7712, 6854, 6483, 6111, 5843, 5490, 5241, 5035, 5001, 4755, 4603, 4498, 4459
cost of process switching: 4459ns


