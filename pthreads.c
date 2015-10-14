#include <stdio.h>
#include <pthread.h>

/**
* Thread two's start function.
**/
void * sayHelloThreadTwo()
{
	printf("Why hello there mate! I'm thread two!\n");
	return NULL;
}

/**
* Thread three's start function.
**/
void * sayHelloThreadThree()
{
	printf("Why hello there mate! I'm thread three!\n");
	return NULL;
}

/**
* The main thread.
**/
int main(int argc, char * argv[])
{
	// Create thread two.
	pthread_t threadTwo;
	pthread_create(&threadTwo, NULL, sayHelloThreadTwo, NULL);

	// Create thread three.
	pthread_t threadThree;
	pthread_create(&threadThree, NULL, sayHelloThreadThree, NULL);

	// Perform main thread operations.
	printf("Why hello there mate! I'm the main thread!\n");

	// Wait for the other threads to end.
	pthread_join(threadTwo, NULL);
	pthread_join(threadThree, NULL);
	
	return 0;
}