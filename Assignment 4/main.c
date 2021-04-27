#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

// Adapted from <CS 344> <main.c> https://repl.it/@cs344/65prodconspipelinec
// Size of the buffer
#define SIZE 50

// Size of each individual line
#define stringSize 1000

// Global Variables
// Buffer 1, shared resource between input thread and square-root thread
char* buffer_1[SIZE];
// Number of items in the buffer
int count_1 = 0;
// Index where the input thread will put the next item
int prod_idx_1 = 0;
// Index where the square-root thread will pick up the next item
int con_idx_1 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;

// Buffer 2, Line separator
char* buffer_2[SIZE];
// Number of items in the buffer
int count_2 = 0;
// Index where the line seperator will put the next item
int prod_idx_2 = 0;
// Index where the plus sign will pick up the next item
int con_idx_2 = 0;
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;

// Buffer 3, Sign thread
char* buffer_3[SIZE];
// Number of items in buffer
int count_3 = 0;
// Index where the plus sign will put the next item
int prod_idx_3 = 0;
// Index where the output thread will pick up the next item
int con_idx_3 = 0;
// Initialize the mutex for buffer 3
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 3
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

/*
* Function that the input thread will run.
* Get input from the user
* Check every time the input says 'STOP'
* Adapted from <CS 344> <main.c> https://repl.it/@cs344/65prodconspipelinec
*/
void* getInput(void* args)
{
	char* userInput;
	// Keep running until STOP is inputted
	while(1)
	{
		size_t len = 0;
		getline(&userInput, &len, stdin);
		put_buff_1(userInput);

		// If userInput is STOP, then break
		if (strcmp(userInput, "STOP\n") == 0)
		{
			break;
		}

	}
	return NULL;
}
/*
* Put the user input in buffer_1
* Adapted from <CS 344> <main.c> https://repl.it/@cs344/65prodconspipelinec
*/
void put_buff_1(char* item)
{
	// Lock the mutex before putting the item in the buffer
	pthread_mutex_lock(&mutex_1);
	// Put the item in the buffer
	buffer_1[prod_idx_1] = item;
  // Increment the index where the next item will be put.
	prod_idx_1 = prod_idx_1 + 1;
	count_1++;
	// Signal to the consumer that the buffer is no longer empty
	pthread_cond_signal(&full_1);
  // Unlock the mutex
	pthread_mutex_unlock(&mutex_1);
}

/*
* Get the next item from buffer 1
* Adapted from <CS 344> <main.c> https://repl.it/@cs344/65prodconspipelinec
*/
char* get_buff_1()
{
	// Lock the mutex before checking if the buffer has data
	pthread_mutex_lock(&mutex_1);
	while (count_1 == 0)
	{
		// Buffer is empty. Wait for the producer to signal that the buffer has data
		pthread_cond_wait(&full_1, &mutex_1);
	}
	char* item = buffer_1[con_idx_1];
	// Increment the index from which the item will be picked up
	con_idx_1 = con_idx_1 + 1;
	count_1--;
	// Unlock the mutex
	pthread_mutex_unlock(&mutex_1);
	// Returns the item
	return item;
}

/*
* Grab item from buffer 1, replace '\n' with ' ' and store in buffer_2
* Adapted from <CS 344> <main.c> https://repl.it/@cs344/65prodconspipelinec
*/
void* lineSep(void* args)
{
	while (1)
	{
		// Store item from buffer1 to the item
		char* item = get_buff_1();

		// Keep running until STOP is inputted
		if (strcmp(item, "STOP\n") == 0)
		{
			put_buff_2(item);
			break;
		}
		for (int i = 0; i < strlen(item); i++)
		{
			if (item[i] == '\n')
			{
				item[i] = ' ';
			}
		}
		put_buff_2(item);
	}
	return NULL;
}
/*
* Put an item in buffer2
* Adapted from <CS 344> <main.c> https://repl.it/@cs344/65prodconspipelinec
*/

void put_buff_2(char* item)
{
  // Lock the mutex before putting the item in the buffer
	pthread_mutex_lock(&mutex_2);
  // Put the item in the buffer
	buffer_2[prod_idx_2] = item;
  // Increment the index where the next item will be put.
	prod_idx_2 = prod_idx_2 + 1;
	count_2++;
  // Signal to the consumer that the buffer is no longer empty
	pthread_cond_signal(&full_2);
	// Unlock the mutex
	pthread_mutex_unlock(&mutex_2);
}

/*
* Get the next item from buffer 1
* Adapted from <CS 344> <main.c> https://repl.it/@cs344/65prodconspipelinec
*/
char* get_buff_2()
{
	// Lock the mutex before checking if the buffer has data
	pthread_mutex_lock(&mutex_2);
	while (count_2 == 0)
	{
		// Buffer is empty. Wait for the producer to signal that the buffer has data
		pthread_cond_wait(&full_2, &mutex_2);
	}
	char* item = buffer_2[con_idx_2];
	// Increment the index from which the item will be picked up
	con_idx_2 = con_idx_2 + 1;
	count_2--;
	// Unlock the mutex
	pthread_mutex_unlock(&mutex_2);
	// Return the item
	return item;
}
/*
* Replace a pair of plus signs to '^'
* Grab from buffer2 and store to buffer3
*/
void* changeSign(void* args)
{
	// Infinitely running until break
	while (1)
	{
		// Store item from buffer2 to item
		char* item = get_buff_2();

		// https://cboard.cprogramming.com/c-programming/73365-how-use-strstr-find-all-occurrences-substring-string-not-only-first.html
		while (strstr(item, "++") != NULL)
		{
			// https://stackoverflow.com/questions/11365039/will-strlen-be-calculated-multiple-times-if-used-in-a-loop-condition
			char final[stringSize] = "";
			int size = 0;
			for (int i = 0; i < strlen(item); i++)
			{
				if (item[i] == '+' && item[i + 1] == '+')
				{
					size = i;
					break;
				}
			}
			strncpy(final, item, size);
			strcat(final, "^");
			strcat(final, (item + size) + 2);
			strcpy(item, final);
		}

		//Store item into buffer3
		put_buff_3(item);

		// Keep running until STOP is inputted
		if (strcmp(item, "STOP\n") == 0)
		{
			break;
		}
	}
	return NULL;
}
/*
* Put user input in buffer3
* Adapted from <CS 344> <main.c> https://repl.it/@cs344/65prodconspipelinec
*/
void put_buff_3(char* item)
{
	// Lock mutex before putting item in the buffer
	pthread_mutex_lock(&mutex_3);
	// Item into buffer
	buffer_3[prod_idx_3] = item;
	// Increment the index where the next item will be put
	prod_idx_3 = prod_idx_3 + 1;
	count_3++;
	// Signals to the consumer that the buffer is no longer empty
	pthread_cond_signal(&full_3);
	// Unlock the mutex
	pthread_mutex_unlock(&mutex_3);
}

/*
* Get the next item from buffer 3 and return
* Adapted from <CS 344> <main.c> https://repl.it/@cs344/65prodconspipelinec
*/
char* get_buff_3()
{
	// Lock the mutex before checking if the buffer has data
	pthread_mutex_lock(&mutex_3);
	while (count_3 == 0)
	{
		// Buffer is empty. Wait for the producer to signal that the buffer has data
		pthread_cond_wait(&full_3, &mutex_3);
	}
	char* item = buffer_3[con_idx_3];
	// Increment the index from which the item will be picked up
	con_idx_3 = con_idx_3 + 1;
	count_3--;
	// Unlock the mutex
	pthread_mutex_unlock(&mutex_3);
	// Return the item
	return item;
}

// Grab item from buffer3 and store to output 80 characters

void* write_output(void* args)
{
	int count = 0;
	while (1)
	{
		char finalBuffer[stringSize];
		char tempBuffer[stringSize];
		// STore 80 characters
		char buf[81];

		int enter = 0;

		char* item = get_buff_3();

		// If userInput is STOP, then break
		if (strcmp(item, "STOP\n") == 0)
		{
			memset(finalBuffer, '\0', sizeof(finalBuffer));
			memset(buf, '\0', sizeof(buf));
			break;
		}
		strcat(finalBuffer, item);
		int size = strlen(finalBuffer);
		float divis = (float)size / 80.0;

		for (int i = 0; i < strlen(finalBuffer); i++)
		{
			// Store the input into buffer
			buf[count] = finalBuffer[i];

			// Increment the counter by 1
			count++;

			// If counter equals 80, output and then delete
			if (count == 80)
			{
				printf("%s\n", buf);
				fflush(stdout);
				memset(buf, '\0', sizeof(buf));

				count = 0;
				enter++;
			}

			// Check if there are characters left after inputting 80 characters
			if((divis - enter) > 0 && enter == (int)divis)
			{
				int tempIter = 0;

				// Start from beginning till the end
				for (int j = i + 1; j < strlen(finalBuffer); j++)
				{
					// finalBuffer leftover values copied
					tempBuffer[tempIter] = finalBuffer[j];
					tempIter++;
				}
				// finalBuffer cleared
				memset(finalBuffer, '\0', sizeof(finalBuffer));
				strcat(finalBuffer, tempBuffer);
				memset(tempBuffer, '\0', sizeof(tempBuffer));

				break;
			}
		}
	}
	return NULL;
}

/*
* Main Function
* Adapted from <CS 344> <main.c> https://repl.it/@cs344/65prodconspipelinec
*/
int main()
{
	// Declare the threads
	pthread_t inputThread, lineSeparator, plusSign, outputThread;

	// Create 4 threads and wait until they're fully terminated
	pthread_create(&inputThread, NULL, getInput, NULL);
	pthread_create(&lineSeparator, NULL, lineSep, NULL);
	pthread_create(&plusSign, NULL, changeSign, NULL);
	pthread_create(&outputThread, NULL, write_output, NULL);
	pthread_join(inputThread, NULL);
	pthread_join(lineSeparator, NULL);
	pthread_join(plusSign, NULL);
	pthread_join(outputThread, NULL);
	memset(buffer_1, '\0', sizeof(buffer_1));
	memset(buffer_2, '\0', sizeof(buffer_2));
	memset(buffer_3, '\0', sizeof(buffer_3));

	return EXIT_SUCCESS;
}
