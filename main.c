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


pthread_mutex_t count_mutex;


void *pthreadMutexTest()
{
	
    int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		pthread_mutex_lock(&count_mutex);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		pthread_mutex_unlock(&count_mutex);    
	
    }   

    pthread_exit(NULL);
}

int runTest(int testID)
{

/*You must create all data structures and threads for you experiments*/

if (testID == 0 || testID == 1 ) /*Pthread Mutex*/
{
	c=0;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for(i=0;i<numThreads;i++)
	{
	
	 if( (rt=(pthread_create( threads+i, NULL, &pthreadMutexTest, NULL)) ) )
	{
		printf("Thread creation failed: %d\n", rt);
		return -1;	
	}
	
	}
	
	for(i=0;i<numThreads;i++) //Wait for all threads to finish
	{
		 pthread_join(threads[i], NULL);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

	printf("Threaded Run Pthread (Mutex) Total Count: %lld\n", c);
	result=timespecDiff(&stop,&start);
	printf("Pthread Mutex time(ms): %llu\n",result/1000000);

}

if(testID == 0 || testID == 2) /*Pthread Spinlock*/
{
/*Pthread Spinlock goes here*/
}

if(testID == 0 || testID == 3) /*MySpinlockTAS*/
{
/* mySpinlock TAS goes here*/
}

/*....you must implement the other tests....*/

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
	numItterations=1000000;
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
			break;
		}

		flagIntValue = atoi(flagValue);

		if (strcmp("-t", flagInput) == 0) {
			printf("flag is -t | value is: %s\n", flagValue);
			numThreads = flagIntValue;
		} else if (strcmp("-i", flagInput) == 0) {
			printf("flag is -i | value is: %s\n", flagValue);
			numItterations = flagIntValue;
		} else if (strcmp("-o", flagInput) == 0) {
			printf("flag is -o | value is: %s\n", flagValue);
			workOutsideCS = flagIntValue;
		} else if (strcmp("-c", flagInput) == 0) {
			printf("flag is -c | value is: %s\n", flagValue);
			workInsideCS = flagIntValue;
		} else if (strcmp("-d", flagInput) == 0) {
			printf("flag is -d | value is: %s\n", flagValue);
			testID = flagIntValue;
		}

		i += 2;
	}
	
	return 0;
}

int main(int argc, char *argv[])
{


	printf("Usage of: %s -t #threads -i #Itterations -o #OperationsOutsideCS -c #OperationsInsideCS -d testid\n", argv[0]);
	printf("testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock, \n");	
	
	//testAndSetExample(); //Uncomment this line to see how to use TAS
	
	processInput(argc,argv);
	runTest(testID);
	return 0;

}
