#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define ONE_BILLION 1000000000

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p);
void print(char* message, unsigned long long result);
void minFcnCall();

int main() {

  struct timespec startTime;
  struct timespec endTime;
  unsigned long long result;

  // question 1: measure cost of minimal function call
  clock_gettime(CLOCK_MONOTONIC, &startTime);
  minFcnCall();
  clock_gettime(CLOCK_MONOTONIC, &endTime);
  result = timespecDiff(&startTime, &endTime);
  print("cost of minimal function call", result);

  return 0;
}

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{ 
  //printf("timeA_p->tv_sec: %llu\n", (long long unsigned)timeA_p->tv_sec);
  //printf("timeA_p->tv_nsec: %llu\n", (long long unsigned) timeA_p->tv_nsec);
  //printf("timeB_p->tv_sec: %llu\n", (long long unsigned)timeB_p->tv_sec);
  //printf("timeB_p->tv_nsec: %llu\n", (long long unsigned)timeB_p->tv_nsec);
  return abs(((timeA_p->tv_sec * ONE_BILLION) + timeA_p->tv_nsec) -
                ((timeB_p->tv_sec * ONE_BILLION) + timeB_p->tv_nsec));
}

void print(char* message, unsigned long long result) { 
  unsigned long long s = result / ONE_BILLION;
  unsigned long long ms = result % ONE_BILLION;
  printf("%s: %llus %llums\n", message, s, ms);
}

void minFcnCall() {}

