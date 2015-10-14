#include <stdio.h>
#include <pthread.h>

// Defining the max size of the string buffer.
#define BUFFER_SIZE 60

// Creating the character array buffer.
char stringBuffer[BUFFER_SIZE];

// The current index within the character array buffer.
int buffIndex = 0;

// The boolean to tell if we are still reading the line of text.
int stillReading = 0;

// TEMP
char * temp = "hello\0";

// Creating the temporary character buffer.
char charBuffer;

// Declaring my mutex for reading and printing characters.
static pthread_mutex_t readCharMutex;
static pthread_mutex_t printCharMutex;

/**
* Thread two's start function.
**/
void * sayHelloThreadTwo()
{
	// Locking the mutexes for printing characters.
	pthread_mutex_lock(&printCharMutex);

	while(stillReading)
	{
		// Lock the mutex for reading characters.
		pthread_mutex_lock(&readCharMutex);

		// Performing logic for reading characters.
		printf("Storing character %c.\n", charBuffer);

		// Adding the characters to the string buffer.
		stringBuffer[buffIndex++] = charBuffer;

		// Unlocking the mutexes for reading and printing characters.
		pthread_mutex_unlock(&readCharMutex);
	}

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
	//printf("Why hello there mate! I'm thread three!\n\n");
	printf("%s\n", stringBuffer);
	buffIndex = 0;

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

	// Setting the still reading boolean to be true.
	stillReading = 1;

	// Create thread two.
	pthread_t threadTwo;
	pthread_create(&threadTwo, NULL, sayHelloThreadTwo, NULL);

	// Create thread three.
	pthread_t threadThree;
	pthread_create(&threadThree, NULL, sayHelloThreadThree, NULL);

	for(int i = 0; i <= 5; i++)
	{
		// If this is the first iteration through the loop, the mutex is already
		// locked.
		if(i != 0)
		{
			// Locking the reading character mutex.
			pthread_mutex_lock(&readCharMutex);
		}

		// Perform main thread operations.
		printf("Added character number %d.\n", i);
		charBuffer = temp[i];

		pthread_mutex_unlock(&readCharMutex);
	}

	// Letting thread two know that we are done reading the line.
	stillReading = 0;

	// Wait for the other threads to end.
	pthread_join(threadTwo, NULL);
	pthread_join(threadThree, NULL);
	
	pthread_mutex_destroy(&readCharMutex);
	pthread_mutex_destroy(&printCharMutex);
	return 0;
}