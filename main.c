#include "main.h"

void *fnC()
{
    int i;
    for(i=0;i<1000000;i++)
    {   
        c++;
    }   

    pthread_exit(NULL);
}

void *pthreadMutexTest()
{
  int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i = 0; i < numItterations; i++)
    {
		
      for(j = 0; j < workOutsideCS; j++)/*How much work is done outside the CS*/
      {
        localCount++;
      }
      
      pthread_mutex_lock(&count_mutex);
      for(k = 0; k < workInsideCS; k++)/*How much work is done inside the CS*/
      {
        c++;
      }
      pthread_mutex_unlock(&count_mutex);    
	
    }   

    pthread_exit(NULL);
}

void *spinLockTest()
{
  int i;
	int j;
	int k;
	
	int localCount = 0;
	
	if (pthread_spin_init(&pSpinlock, PTHREAD_PROCESS_PRIVATE) != 0)
  {
    printf("error occurred for pthread_spin_init\n");
    return NULL; 
  }
  
  for(i = 0; i < numItterations; i++)
  {
   
    for(j = 0; j < workOutsideCS; j++) /* How much work is done outside the CS */
    { 
      localCount++;
    }
    
    while (pthread_spin_lock(&pSpinlock));
    for(k = 0; k < workInsideCS; k++) /* How much work is done inside the CS */
    {
      c++;
    }
    pthread_spin_unlock(&pSpinlock);

  }

  pthread_exit(NULL);
}

void *mySpinLockTASTest()
{	
  int i;
	int j;
	int k;
	
	int localCount = 0;
	
  for(i = 0; i < numItterations; i++)
  {
		
		for(j = 0; j < workOutsideCS; j++) /* How much work is done outside the CS */
		{
			localCount++;
		}
		
		my_spinlock_lockTAS(&mSpinlock);
		for(k = 0; k < workInsideCS; k++) /* How much work is done inside the CS */
		{
			c++;
		}
		my_spinlock_unlock(&mSpinlock);    
	
  } 

  pthread_exit(NULL);
}

void *mySpinLockTTASTest()
{	
  int i;
	int j;
	int k;
	
	int localCount = 0;
	
  for(i = 0; i < numItterations; i++)
  {
  
    for(j = 0; j < workOutsideCS; j++)/*How much work is done outside the CS*/
    {
      localCount++;
    }
    
    my_spinlock_lockTTAS(&mSpinlock); 
    for(k = 0; k < workInsideCS; k++)/*How much work is done inside the CS*/
    {
      c++;
    }
    my_spinlock_unlock(&mSpinlock);    
  
  }   

  pthread_exit(NULL);
}

void *myMutexTTASTest()
{	
  int i;
	int j;
	int k;
	
	int localCount = 0;
	
  for(i = 0; i < numItterations; i++)
  {
  
    for(j = 0; j < workOutsideCS; j++)/*How much work is done outside the CS*/
    {
      localCount++;
    }
    
    my_mutex_lock(&mMutexlock); 
    for(k = 0; k < workInsideCS; k++)/*How much work is done inside the CS*/
    {
      c++;
    }
    my_mutex_unlock(&mMutexlock);    
  
  }   

  pthread_exit(NULL);
}

void recursiveSpinLockTestFcn(int n) {
  int j;
  int k;

  int localCount = 0;

  pthread_t pid = pthread_self();
  if (n <= 0) { 
//    printf("\n\tpid[%d] Hit 0 --> returning...", pid);
    return; 
  }
  for (j = 0; j < workOutsideCS; j++) {
    localCount++;
  }
//  printf("\n\tpid[%d] Attempting to acquire lock | n: %d", pid, n);
  my_spinlock_lockTAS(&mSpinlock);
  for (k = 0; k < workInsideCS; k++) {
    c++;
  }
//  printf("\n\t\tpid[%d] [1] Acquired lock -> calling recursiveSpinLockTestFcn(%d)", pid, n-1);
  recursiveSpinLockTestFcn(n-1);
//  printf("\n\tpid[%d] [2] Attempting to unlock", pid);
  my_spinlock_unlock(&mSpinlock);
//  printf("\n\tpid[%d] [3] After unlock | n: %d", pid, n);
}

void *myRecursiveSpinLockTASTest()
{
  recursiveSpinLockTestFcn(numItterations); 
}

int runTestWithPthread(char* testName, void *(*f)(void*)) 
{
	c = 0;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t*)malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for(i = 0; i < numThreads; i++)
	{
		if ( (rt = (pthread_create(threads+i, NULL, f, NULL))) )
		{
			printf("Thread creation failed: %d\n", rt);
			return -1;	
		}
	}
	
	for(i = 0; i < numThreads; i++) //Wait for all threads to finish
	{
		 pthread_join(threads[i], NULL);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

	printf("\nThreaded Run Pthread (%s) Total Count: %lld\n", testName, c);
	result=timespecDiff(&stop,&start);
	printf("Pthread %s time(ms): %llu\n", testName, result/1000000);
	return 0;
}

int runTest(int testID)
{

	/*You must create all data structures and threads for you experiments*/

	// Pthread Mutex
	if (testID == 0 || testID == 1)
	{
		runTestWithPthread("Mutex", &pthreadMutexTest);
	}

	// Pthread Spinlock
	if(testID == 0 || testID == 2) 
	{
		/* Pthread Spinlock goes here */
		runTestWithPthread("Professional Spinlock", &spinLockTest);
	}

 	// MySpinlock TAS
	if(testID == 0 || testID == 3)
	{
	  my_spinlock_init(&mSpinlock);
		runTestWithPthread("MySpinlock TAS", &mySpinLockTASTest);
	}

	// MySpinlock TTAS
	if (testID == 0 || testID == 4)
	{
	  my_spinlock_init(&mSpinlock);
		runTestWithPthread("MySpinlock TTAS", &mySpinLockTTASTest);
	}

	// MyMutex TTAS
	if (testID == 0 || testID == 5)
	{
	  my_mutex_init(&mMutexlock);
    runTestWithPthread("MyMutex TTAS test", &myMutexTTASTest); 
	}

	// MyQueueLock
	if (testID == 0 || testID == 6)
	{

	}

  // Recursive MySpinlock TAS
  if (testID == 0 || testID == 7)
  {
    my_spinlock_init(&mSpinlock);
    runTestWithPthread("Recursive MySpinlock TAS", &myRecursiveSpinLockTASTest);
  }

	return 0;
}

int testAndSetExample()
{
	volatile unsigned long test = 0; //Test is set to 0
	printf("Test before atomic OP:%ld\n",test);
	tas(&test);
	printf("Test after atomic OP:%ld\n",test);
	pthread_exit(NULL);
}

int processInput(int argc, char *argv[])
{

	/*testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock*/
	/*You must write how to parse input from the command line here, your software should default to the values given below if no input is given*/
	
	numThreads=4;
	numItterations=10000;
	testID=0;
	workOutsideCS=0;
	workInsideCS=1;

	char* flagInput;
	char* flagValue;
	int i;
	int j;
	int isValid;
	int flagIntValue;

	i = 1;
	isValid = 1;
	while (i < argc) {
		flagInput = argv[i];

		flagValue = argv[i+1];

		if (flagValue == NULL) {
			isValid = 0;
		} else {
			isValid = 1;
			for (j = 0; j < strlen(flagValue); j++) {
				if (!isValid) break;
				if (!isdigit(flagValue[j])) {
					isValid = 0;
				}
			}	
		}
		if (!isValid) {
			printf("Requires a valid input for the flag %s\n", flagInput);
			return -1;
		}

		flagIntValue = atoi(flagValue);

		if (strcmp("-t", flagInput) == 0) {
			printf("\tDEBUG - flag is -t | value is: %s\n", flagValue);
			numThreads = flagIntValue;
		} else if (strcmp("-i", flagInput) == 0) {
			printf("\tDEBUG - flag is -i | value is: %s\n", flagValue);
			numItterations = flagIntValue;
		} else if (strcmp("-o", flagInput) == 0) {
			printf("\tDEBUG - flag is -o | value is: %s\n", flagValue);
			workOutsideCS = flagIntValue;
		} else if (strcmp("-c", flagInput) == 0) {
			printf("\tDEBUG - flag is -c | value is: %s\n", flagValue);
			workInsideCS = flagIntValue;
		} else if (strcmp("-d", flagInput) == 0) {
			printf("\tDEBUG - flag is -d | value is: %s\n", flagValue);
			testID = flagIntValue;
		}

		i += 2;
	}
	
	return 0;
}

int main(int argc, char *argv[])
{
	int status;

  // for seeding random numbers
  srand(time(NULL));

	printf("Usage of: %s -t #threads -i #Itterations -o #OperationsOutsideCS -c #OperationsInsideCS -d testid\n", argv[0]);
	printf("testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock, \n");	
	
	//testAndSetExample(); //Uncomment this line to see how to use TAS
	
	status = processInput(argc,argv);
	if (status != 0) return 1;
	runTest(testID);
	return 0;

}
