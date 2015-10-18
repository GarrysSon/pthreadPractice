#include <stdio.h>
#include <pthread.h>

// Defining the max size of the string buffer.
#define BUFFER_SIZE 60

// Defining the control characters.
#define PRINT_BUFFER '@'
#define ERASE_LAST_CHAR '*'
#define DELETE_LAST_WORD '$'
#define CLEAR_BUFFER '&'

// Creating the character array buffer.
char stringBuffer[BUFFER_SIZE];

// The current index within the character array buffer.
int buffIndex = 0;

// The boolean to tell if we are still reading the file or a the line of text.
int readingFile = 0;

// The boolean to tell if we are reading a character, initially true.
int readingChar = 1;

// The boolean to tell if we are printing a character, initially false.
int printingChar = 0;

// Creating the temporary character buffer.
char charBuffer;

// Declaring my mutex for reading, storing, and printing characters.
static pthread_mutex_t mutex;

// Declaring my conditions for reading, storing, and printing characters.
static pthread_cond_t fillCharCond;
static pthread_cond_t storeCharCond;
static pthread_cond_t printCharCond;

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
	// Iterate over the loop until the line is read.
	while(readingFile)
	{
		// Locking on the mutex.
		pthread_mutex_lock(&mutex);

		// Wait until it is time to print again.
		while(readingChar)
		{
			pthread_cond_wait(&storeCharCond, &mutex);
		}

		// See what action should be taken based on the current character.
		switch(charBuffer)
		{
			case PRINT_BUFFER:
				// Setting the printing character boolean to true.
				printingChar++;

				// Print the contents of the buffer.
				pthread_cond_signal(&printCharCond);
				pthread_mutex_unlock(&mutex);

				// Wait until we are done printing the characters.
				while(printingChar)
				{
					pthread_cond_wait(&printCharCond, &mutex);
				}
				break;
			case ERASE_LAST_CHAR:
				if(buffIndex > 0)
				{
					// Decrement the buffer index by one.
					buffIndex--;
				}
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
		stringBuffer[buffIndex] = '\n';
		stringBuffer[buffIndex + 1] = '\0';

		// Set the reading character boolean to true.
		readingChar++;

		// Signal for another character to be read.
		pthread_cond_signal(&fillCharCond);
		pthread_mutex_unlock(&mutex);
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
		pthread_mutex_lock(&mutex);

		// Wait until it is time to print the characters.
		while(!printingChar)
		{
			pthread_cond_wait(&printCharCond, &mutex);
		}

		// Performing logic for writing characters.
		printf("%s", stringBuffer);
		buffIndex = 0;
		charBuffer = '\0';

		// Setting the printing characters boolean to false.
		printingChar--;

		// Signal that we are done printing.
		pthread_cond_signal(&printCharCond);
		pthread_mutex_unlock(&mutex);
	}

	return NULL;
}

/**
* The main thread.
**/
int main(int argc, char * argv[])
{
	// The temporary character variable.
	char temp;

	// Open the file.
	FILE * file;
	file = fopen("hw4input.txt", "rt");

	// Check that the file was successfully opened.
	if(!file)
	{
		printf("The file was not found!\n");
		return 1;
	}

	// Creating the mutex for reading, storing, and printing characters.
	pthread_mutex_init(&mutex, NULL);

	// Creating the conditions for reading, storing, and printing characters.
	pthread_cond_init(&fillCharCond, NULL);
	pthread_cond_init(&storeCharCond, NULL);
	pthread_cond_init(&printCharCond, NULL);

	// Setting the reading from file boolean to true.
	readingFile = 1;

	// Create thread two.
	pthread_t threadTwo;
	pthread_create(&threadTwo, NULL, sayHelloThreadTwo, NULL);

	// Create thread three.
	pthread_t threadThree;
	pthread_create(&threadThree, NULL, sayHelloThreadThree, NULL);

	// Generate the line.
	while((temp = getc(file)) != EOF)
	{
		// Locking on the mutex.
		pthread_mutex_lock(&mutex);

		// Wait until it is time to read a character.
		while(!readingChar)
		{
			pthread_cond_wait(&fillCharCond, &mutex);
		}

		// Move the character from the temp variable to the buffer.
		charBuffer = temp;

		// Set the reading character boolean to false.
		readingChar--;

		// Signal for the character to be stored.
		pthread_cond_signal(&storeCharCond);
		pthread_mutex_unlock(&mutex);
	}

	// The next section is to signal the end of the operation.
	while(!readingChar)
	{
		pthread_cond_wait(&fillCharCond, &mutex);
	}

	// Move the character from the temp variable to the buffer.
	charBuffer = PRINT_BUFFER;

	// Set the reading character boolean to false.
	readingChar--;

	// Signal for the character to be stored.
	pthread_cond_signal(&storeCharCond);
	pthread_mutex_unlock(&mutex);

	// Letting thread two know that we are done reading the file.
	readingFile = 0;

	// Wait for the other threads to end.
	pthread_join(threadTwo, NULL);
	pthread_join(threadThree, NULL);
	
	// Destroying mutexes and conditions.
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&fillCharCond);
	pthread_cond_destroy(&storeCharCond);
	pthread_cond_destroy(&printCharCond);

	fclose(file);

	return 0;
}