#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>

#define ONE_BILLION 1000000000
#define A_THOUSAND 1000

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p);
void printDetailed(char* message, unsigned long long result, unsigned long long arr[], int n);
void print(char* message, unsigned long long result);
unsigned long long measureMinFcnCall(int n, unsigned long long arr[]);
void minFcnCall();
unsigned long long measureMinSystemCall(int n, unsigned long long arr[]);
unsigned long long measureProcessSwitching(int n);
unsigned long long measureThreadSwitching(int n);
void *thread1();
void *thread2();


// global variables
pthread_t t[2];
int sharedInt = 0;
pthread_mutex_t mutex;
pthread_cond_t hasSetOne = PTHREAD_COND_INITIALIZER;
pthread_cond_t hasSetZero = PTHREAD_COND_INITIALIZER;

int main() {
  // http://stackoverflow.com/questions/10490756/how-to-use-sched-getaffinity2-and-sched-setaffinity2-please-give-code-samp
  cpu_set_t set;
  CPU_ZERO(&set);
  CPU_SET(0, &set);
  sched_setaffinity(0, sizeof(cpu_set_t), &set);

  int n = 20;
  unsigned long long minFcnCallResult[n];
  unsigned long long minSystemCallResult[n];
  unsigned long long processSwitchingResult[n];
  unsigned long long threadSwitchingResult[n];
  unsigned long long result;
  int i = 0;
  
  // question 2: measure cost of minimal function call
  result = measureMinFcnCall(n, minFcnCallResult);
  printDetailed("cost of minimal function call", result, minFcnCallResult, n);
  // question 3: measure cost of minimal system call
  result = measureMinSystemCall(n, minSystemCallResult);
  printDetailed("cost of minimal system call", result, minSystemCallResult, n);
  
  // question 4: measure cost of process switching
  while (i < n) {
    result = measureProcessSwitching(i+1);
    processSwitchingResult[i] = result;
    i++;
  }
  printDetailed("cost of process switching", result, processSwitchingResult, n);

  // question 5: measure thread switching
  result = measureThreadSwitching(1);
  threadSwitchingResult[i] = result;
  printf("hi");
  print("cost of thread switching", result);
  return 0;
}

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{ 
  //printf("timeA_p->tv_sec: %llu\n", (long long unsigned)timeA_p->tv_sec);
  //printf("timeA_p->tv_nsec: %llu\n", (long long unsigned) timeA_p->tv_nsec);
  //printf("timeB_p->tv_sec: %llu\n", (long long unsigned)timeB_p->tv_sec);
  //printf("timeB_p->tv_nsec: %llu\n", (long long unsigned)timeB_p->tv_nsec);
  return ((timeA_p->tv_sec * ONE_BILLION) + timeA_p->tv_nsec) -
                ((timeB_p->tv_sec * ONE_BILLION) + timeB_p->tv_nsec);
}

void printDetailed(char* message, unsigned long long result, 
           unsigned long long arr[], int n) {  
  int i = 0;
  while (i < n) {
    (i == (n-1)) ? printf("%llu", arr[i]) : printf("%llu, ", arr[i]);
    i++;
  }
  printf("\n%s: %lluns\n\n", message, result);
}

void print(char* message, unsigned long long result) {
  printf("%s: %lluns\n", message, result);
}

unsigned long long measureMinFcnCall(int n, unsigned long long arr[]) {
  struct timespec startTime;
  struct timespec endTime;
  unsigned long long diff = 0;
  unsigned long long result = 0;

  int i = 0;
  while (i < n) { 
    clock_gettime(CLOCK_MONOTONIC, &startTime);
    minFcnCall();
    clock_gettime(CLOCK_MONOTONIC, &endTime);
    diff = timespecDiff(&endTime, &startTime);
    arr[i] = diff;
    result += diff;
    i++;
  }

  return result / n;
}

void minFcnCall() {}

unsigned long long measureMinSystemCall(int n, unsigned long long arr[]) {
  struct timespec startTime;
  struct timespec endTime;
  unsigned long long diff = 0;
  unsigned long long result = 0;

  int i = 0;
  while (i < n) { 
    clock_gettime(CLOCK_MONOTONIC, &startTime);
    getpid();
    clock_gettime(CLOCK_MONOTONIC, &endTime);
    diff = timespecDiff(&endTime, &startTime);
    arr[i] = diff;
    result += diff;
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

  result = timespecDiff(&endTime, &startTime);

  return result / ( (2 * n) - 1 );
}

unsigned long long measureThreadSwitching(int n) {  
  struct timespec startTime;
  struct timespec endTime;
  unsigned long long result = 0;

  int i = 0;
  while (i < n) {
    clock_gettime(CLOCK_MONOTONIC, &startTime);
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&t[1], NULL, thread2, NULL);
    pthread_create(&t[0], NULL, thread1, NULL);
    pthread_exit(NULL); 
    clock_gettime(CLOCK_MONOTONIC, &endTime);
    result += timespecDiff(&endTime, &startTime);
    i++;
  }
 
  return result / n;
}

void *thread1() {
  pthread_mutex_lock(&mutex);
  while (sharedInt == 0) {
    pthread_cond_wait(&hasSetZero, &mutex);
    sharedInt = 1;
  }
  pthread_mutex_unlock(&mutex);
  pthread_cond_signal(&hasSetOne);
  return NULL;
}

void *thread2() {
  pthread_mutex_lock(&mutex);
  while (sharedInt == 1) { 
    pthread_cond_wait(&hasSetOne, &mutex);
  }
  sharedInt = 0;
  pthread_mutex_unlock(&mutex);
  pthread_cond_signal(&hasSetZero);
  return NULL;
}

