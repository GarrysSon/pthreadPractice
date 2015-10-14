#include <stdio.h>
#include <pthread.h>

// Defining the max size of the string buffer.
#define BUFFER_SIZE 60

// Creating the character array buffer.
char stringBuffer[BUFFER_SIZE];

// Creating the temporary character buffer.
char * charBuffer;

// Declaring my mutex for reading and printing characters.
static pthread_mutex_t readCharMutex;
static pthread_mutex_t printCharMutex;

/**
* Thread two's start function.
**/
void * sayHelloThreadTwo()
{
	// Locking the mutexes for reading and printing characters.
	pthread_mutex_lock(&printCharMutex);
	pthread_mutex_lock(&readCharMutex);

	// Performing logic for reading characters.
	printf("Why hello there mate! I'm thread two!\n");

	// Adding the characters to the string buffer.
	stringBuffer[0] = 'h';
	stringBuffer[1] = 'e';
	stringBuffer[2] = 'l';
	stringBuffer[3] = 'l';
	stringBuffer[4] = 'o';
	stringBuffer[5] = '\0';

	// Unlocking the mutexes for reading and printing characters.
	pthread_mutex_unlock(&readCharMutex);
	pthread_mutex_unlock(&printCharMutex);

	return NULL;
}

/**
* Thread three's start function.
**/
void * sayHelloThreadThree()
{
	// Locking the mutex for printing characters.
	pthread_mutex_lock(&printCharMutex);

	// Performing logic for writing characters.
	printf("Why hello there mate! I'm thread three!\n\n");
	printf("%s\n", stringBuffer);

	// Unlocking the mutex for printing characters.
	pthread_mutex_unlock(&printCharMutex);

	return NULL;
}

/**
* The main thread.
**/
int main(int argc, char * argv[])
{
	// Creating the mutex for reading in characters.
	pthread_mutex_init(&readCharMutex, NULL);
	pthread_mutex_init(&printCharMutex, NULL);

	// Locking the reading character mutex.
	pthread_mutex_lock(&readCharMutex);

	// Create thread two.
	pthread_t threadTwo;
	pthread_create(&threadTwo, NULL, sayHelloThreadTwo, NULL);

	// Create thread three.
	pthread_t threadThree;
	pthread_create(&threadThree, NULL, sayHelloThreadThree, NULL);

	// Perform main thread operations.
	printf("Why hello there mate! I'm the main thread!\n");

	pthread_mutex_unlock(&readCharMutex);

	// Wait for the other threads to end.
	pthread_join(threadTwo, NULL);
	pthread_join(threadThree, NULL);
	
	pthread_mutex_destroy(&readCharMutex);
	pthread_mutex_destroy(&printCharMutex);

	return 0;
}