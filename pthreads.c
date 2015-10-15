#include <stdio.h>
#include <pthread.h>

// Defining the max size of the string buffer.
#define BUFFER_SIZE 60

// Defining the control characters.
#define END_OF_LINE '@'
#define ERASE_LAST_CHAR '*'
#define DELETE_LAST_WORD '$'
#define CLEAR_BUFFER '&'

// Creating the character array buffer.
char stringBuffer[BUFFER_SIZE];

// The current index within the character array buffer.
int buffIndex = 0;

// The boolean to tell if we are still reading the file or a the line of text.
int readingFile = 0;

// TEMP
char * temp = "hello@Hello@";

// Creating the temporary character buffer.
char charBuffer;

// Declaring my mutex for reading and printing characters.
static pthread_mutex_t readCharMutex;
static pthread_mutex_t printCharMutex;

/**
* Gets the index of the beginning of the last word.
**/
int GetLastWordIndex(int currentIndex)
{
	// Iterate over the buffer until we find a space character.
	while(currentIndex > 0 && stringBuffer[currentIndex] != ' ')
	{
		currentIndex--;
	}

	// Return the found index.
	return currentIndex;
}

/**
* Thread two's start function.
**/
void * sayHelloThreadTwo()
{
	// Locking the mutexes for printing characters.
	pthread_mutex_lock(&printCharMutex);

	// Iterate over the loop until the line is read.
	while(readingFile)
	{
		// Lock the mutex for reading characters.
		pthread_mutex_lock(&readCharMutex);

		// Performing logic for reading characters.
		printf("Storing character %c.\n", charBuffer);

		// See what action should be taken based on the current character.
		switch(charBuffer)
		{
			case END_OF_LINE:
				// Print the contents of the buffer.
				pthread_mutex_unlock(&printCharMutex);
				pthread_mutex_lock(&printCharMutex);
				break;
			case ERASE_LAST_CHAR:
				// Decrement the buffer index by one.
				buffIndex--;
				break;
			case DELETE_LAST_WORD:
				// Get the index of the beginning of the last word in the buffer.
				buffIndex = GetLastWordIndex(buffIndex);
				break;
			case CLEAR_BUFFER:
				// Reset the buffer index.
				buffIndex = 0;
				break;
			default:
				// Adding the characters to the string buffer.
				stringBuffer[buffIndex++] = charBuffer;
				break;
		}

		// Add the NULL character to the end of the current string in the buffer.
		stringBuffer[buffIndex] = '\0';

		// Unlocking the mutex for reading characters.
		pthread_mutex_unlock(&readCharMutex);
	}

	return NULL;
}

/**
* Thread three's start function.
**/
void * sayHelloThreadThree()
{
	while(readingFile)
	{
		// Locking the mutex for printing characters.
		pthread_mutex_lock(&printCharMutex);

		// Performing logic for writing characters.
		printf("%s\n", stringBuffer);
		buffIndex = 0;

		// Unlocking the mutex for printing characters.
		pthread_mutex_unlock(&printCharMutex);
	}

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

	// Setting the reading from file boolean to true.
	readingFile = 1;

	// Create thread two.
	pthread_t threadTwo;
	pthread_create(&threadTwo, NULL, sayHelloThreadTwo, NULL);

	// Create thread three.
	pthread_t threadThree;
	pthread_create(&threadThree, NULL, sayHelloThreadThree, NULL);

	// Generate the line.
	for(int i = 0; i <= 11; i++)
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

	// Letting thread two know that we are done reading the file.
	readingFile = 0;

	// Wait for the other threads to end.
	pthread_join(threadTwo, NULL);
	pthread_join(threadThree, NULL);
	
	pthread_mutex_destroy(&readCharMutex);
	pthread_mutex_destroy(&printCharMutex);
	return 0;
}