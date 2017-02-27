#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#define ONE_BILLION 1000000000
#define ONE_HUNDRED_MILLION 100000000

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p);
void print(char* message, unsigned long long result);
unsigned long long measureMinFcnCall(int n);
void minFcnCall();
unsigned long long measureMinSystemCall(int n);
unsigned long long measureProcessSwitching(int n);

int main() {
  unsigned long long result;

/*
  // question 2: measure cost of minimal function call
  result = measureMinFcnCall(ONE_HUNDRED_MILLION);
  print("cost of minimal function call", result);
  // question 3: measure cost of minimal system call
  result = measureMinSystemCall(ONE_HUNDRED_MILLION);
  print("cost of minimal system call", result);
  */
  // question 4: measure cost of process switching
  result = measureProcessSwitching(2);
  print("cost of process switching", result);

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

unsigned long long measureMinFcnCall(int n) {
  struct timespec startTime;
  struct timespec endTime;
  unsigned long long result = 0;

  int i = 0;
  while (i < n) { 
    clock_gettime(CLOCK_MONOTONIC, &startTime);
    minFcnCall();
    clock_gettime(CLOCK_MONOTONIC, &endTime);
    result += timespecDiff(&startTime, &endTime);
    i++;
  }

  return result / n;
}

void minFcnCall() {}

unsigned long long measureMinSystemCall(int n) {
  struct timespec startTime;
  struct timespec endTime;
  unsigned long long result = 0;

  int i = 0;
  while (i < n) { 
    clock_gettime(CLOCK_MONOTONIC, &startTime);
    getpid();
    clock_gettime(CLOCK_MONOTONIC, &endTime);
    result += timespecDiff(&startTime, &endTime);
    i++;
  }

  return result / n;
}

unsigned long long measureProcessSwitching(int n) {
  struct timespec startTime;
  struct timespec endTime;
  unsigned long long result = 0;

  char input1 = 'a';
  char input2 = 'b';
  char buffer;

  pid_t pid;
  // pipe going from parent to child process
  int pipe1[2];
  // pipe going from child to parent process
  int pipe2[2]; 
  // pipe for measuring the context switch between parent-child & child-parent
  int pipe3[2];
  pipe(pipe1);
  pipe(pipe2);
  pipe(pipe3);
  pid = fork();
  if (pid == 0) {
    close(pipe1[1]);
    close(pipe2[0]);

    int i;
    for (i = 0; i < n; i++) {
      // block while waiting for input1 from parent (2)
      read(pipe1[0], &buffer, 1);
        // write(STDOUT_FILENO, &buffer, 1);  
        // write(STDOUT_FILENO, "\nfinished child \n", 17);
      // send input2 to parent (3)
      write(pipe2[1], &input2, 1);
    }

    clock_gettime(CLOCK_MONOTONIC, &endTime);
    write(pipe3[1], &endTime, sizeof(struct timespec));

    close(pipe1[0]);
    close(pipe2[1]);
    
    _exit(EXIT_SUCCESS);
  } else {
    close(pipe1[0]);
    close(pipe2[1]);

    // start timer
    clock_gettime(CLOCK_MONOTONIC, &startTime);

    int i;
    for (i = 0; i < n; i++) {
      // send input1 to child (1)
      write(pipe1[1], &input1, 1);
      // block while waiting for input2 from parent (4)
      read(pipe2[0], &buffer, 1);
        // write(STDOUT_FILENO, &buffer, 1);
        // write(STDOUT_FILENO, "\nfinished parent \n\n", 19);
    }

    // block while waiting for input
    read(pipe3[0], &endTime, sizeof(struct timespec));

    close(pipe1[1]);
    close(pipe2[0]);
  }

  result = timespecDiff(&startTime, &endTime);
    
  return result / ( (2 * n) - 1 );
}
