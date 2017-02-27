/*
  Task: learn how to use high resolution timers in Linux and C. Describe what each clock does and where it would be appropriate to use them. Also, briefly comment the sample code with what each unique line does.
*/

/*
  Describe what each clock does and where it would be appropriate to use them.

  CLOCK_REALTIME: Reports the wall clock time. Appropriate for use when used for displaying the current time

  CLOCK_MONOTONIC: Clock that represents elapsed time since some arbitrary fixed point in the past and thus, measures relative real time. Appropriate for use when you need to time how long something takes.

  CLOCK_PROCESS_CPUTIME_ID: Measures amount of CPU time used by process. Appropriate for use when you need to determine how much CPU time is used by a process.

  CLOCK_THREAD_CPUTIME_ID: Measures amount of cpu time used by thread. Appropriate for use when you need to determine how much cpu time is used by a thread.


*/

#include <stdio.h>
#include <stdint.h>
// required to use clock_gettime()
#include <time.h>

// function for computing the difference between two times (elapsed time)
unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  // pad tv_sec with 9 zeros to avoid have to work with decimal values for the result
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}


int main()
{

// struct for keeping track of when the timer starts
struct timespec start;
// struct for keeping track of when the timer ends
struct timespec stop;
// stores the elapsed time computed from timespecDiff()
unsigned long long result; //64 bit integer
// retrieve the start time with the clock for CLOCK_REALTIME
clock_gettime(CLOCK_REALTIME, &start);
// sleep for 1 second
sleep(2);
// retrieve the stop time with the clock for CLOCK_REALTIME
clock_gettime(CLOCK_REALTIME, &stop);
// compute the elapsed time between start and stop time for the clock CLOCK_REALTIME
result=timespecDiff(&stop,&start);
// print out the results for the elapsed time associated to CLOCK_REALTIME
printf("CLOCK_REALTIME Measured: %llu\n",result);

// retrieve the start time with the clock for CLOCK_MONOTONIC
clock_gettime(CLOCK_MONOTONIC, &start);
// sleep for 1 second
sleep(1);
// retrieve the stop time with the clock for CLOCK_MONOTONIC
clock_gettime(CLOCK_MONOTONIC, &stop);
// compute the elapsed time between start and stop time for the clock CLOCK_MONOTONIC
result=timespecDiff(&stop,&start);
// print out the results for the elapsed time associated to CLOCK_MONOTONIC
printf("CLOCK_MONOTONIC Measured: %llu\n",result);

// retrieve the start time with the clock for CLOCK_PROCESS_CPUTIME_ID
clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
// sleep for 1 second
sleep(1);
// retrieve the stop time with the clock for CLOCK_PROCESS_CPUTIME_ID
clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
// compute the elapsed time between start and stop time for the clock CLOCK_PROCESS_CPUTIME_ID
result=timespecDiff(&stop,&start);
// print out the results for the elapsed time associated to CLOCK_PROCESS_CPUTIME_ID
printf("CLOCK_PROCESS_CPUTIME_ID Measured: %llu\n",result);

// retrieve the start time with the clock for CLOCK_TRHEAD_CPUTIME_ID
clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
// sleep for 1 second
sleep(1);
// retrieve the stop time with the clock for CLOCK_THREAD_CPUTIME_ID
clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);
// compute the elapsed time between start and stop time for the clock CLOCK_THREAD_CPUTIME_ID
result=timespecDiff(&stop,&start); 
// print out the results for the elapsed time associated to CLOCK_THREAD_CPUTIME_ID
printf("CLOCK_THREAD_CPUTIME_ID Measured: %llu\n",result);


}
