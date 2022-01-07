/*******************************************************************************************
* File Name          : Kamana_Circular_queue.c
* Description        : Prog8130 - Assignment 3: Circular Queue
*					   
* Author:            : Kamana Sharma(8697409)
* Date:              : July 21,2021
********************************************************************************************/

//#include "stdafx.h"
#include <windows.h>
#include <strsafe.h>
#include <string.h>

DWORD WINAPI myReadThreadFunction(LPVOID lpParam);                                   // function used by read from queue thread
unsigned int putToCircularQueue(char* ptrInputBuffer, unsigned int bufferLength);    // circular queue function to add data to queue
unsigned int getFromCircularQueue(char* ptrOutputBuffer, unsigned int bufferLength); // circular queue function to remove data from queue

#define BUFFER_SIZE         200               // local buffer used for sending or receiving data to/from queue in main and worker thread

#define INPUT_TIMEOUT_MS    5000               // dequeue every 5 seconds

#define CIRCULAR_QUEUE_SIZE 200                // size of the circular queue

// data structure used to keep track of circular queue (DO NOT CHANGE THIS STRUCTURE)
struct myQueueStruct {
	char* ptrBuffer = NULL;                    // pointer to start of the circular queue buffer
	volatile unsigned int queueCount = 0;      // number of characters currently in cirucular queue
	volatile char* ptrCircularHead = NULL;     // location where data is added to queue
	volatile char* ptrCircularTail = NULL;     // loation where data is removed from queue
};

struct myQueueStruct myQueue;                  // create an instance of the circular queue data structure

// DO NOT CHANGE THE CODE IN MAIN
int main()
{
	HANDLE  hThread;            // handle to thread
	char    inputBuffer[BUFFER_SIZE];

	myQueue.ptrBuffer = (char*)calloc(CIRCULAR_QUEUE_SIZE, sizeof(char));
	if (myQueue.ptrBuffer == NULL)
	{
		printf("Error unable to allocate memory for buffer\n");
		exit(-1);
	}

	myQueue.ptrCircularHead = myQueue.ptrCircularTail = myQueue.ptrBuffer;

	// create a thread that will consume the data we type in to demonstrate dequeing the data
	hThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		myReadThreadFunction,   // thread function name
		NULL,                   // argument to thread function (not being used)
		0,                      // use default creation flags 
		NULL);                  // returns the thread identifier (not being used)

	// get a string from the console and queue it to circular queue
	while (1)
	{	
	
		scanf_s("%s", inputBuffer, BUFFER_SIZE);    // get data from console

		// put the data into the circular queue but check if an error (marked by queue function returning 0) occurred
		if (putToCircularQueue(inputBuffer, (unsigned int)strlen(inputBuffer)) == 0)
			printf("Error queuing data\n");
	}
	return 0;
}

// FUNCTION      : putToCircularQueue
// DESCRIPTION   :
//   Put the supplied data into a circular queue
// PARAMETERS    :
//   ptrInputBuffer - a pointer to the buffer to be put into the queue
//   bufferLength   - how many characters are in the buffer being put into the queue
//
// RETURNS       :
//   Number of characters successfully queued (0 on failure)

unsigned int putToCircularQueue(char* ptrInputBuffer, unsigned int bufferLength)
{
	unsigned int numberOfCharsQueued = 0;  // number of characters placed in circular queue

	// add code to implement the adding to the circular queue using the data structures in myQueueStruct
	// and the passed in pointer and data to be added count

	if (bufferLength > numberOfCharsQueued)									//if condition to check if number of characters in queue are greater than bufferlength
	{
		int qCount;															//variable to store counter value after each insertion

		for(qCount = 0 ; qCount < bufferLength ;qCount++ )					//for loop for keeping the count value untill it reacjed bufferlength
		{
			*myQueue.ptrCircularHead = *ptrInputBuffer;						// copying the input buffer data into the head of circular queue
			myQueue.ptrCircularHead++;										//incrementing the head pointer after each insertion
			ptrInputBuffer++;												//incrementing the input buffer
			numberOfCharsQueued++;											//incrementing the numbers of characters already queued
			

			if (CIRCULAR_QUEUE_SIZE < numberOfCharsQueued)					//condition to check if head pointer reached the limit or not
			{
				myQueue.ptrCircularHead = myQueue.ptrBuffer;				//if yes, the buffer will be assigned to pointer head
			}
			myQueue.queueCount++;											//incrementing the queuecount of the structure
		}
	}
	
	else
	{
		printf("\n Queue Is Full....\n");									//else condition when the queue is full
	}
	
	return numberOfCharsQueued;
}

// FUNCTION      : getFromCircularQueue
// DESCRIPTION   :
//   Read as much data as there is room for from the circular queue
// PARAMETERS    :
//   ptrOutputBuffer - a pointer to the buffer to place the data read from queue
//   bufferLength    - maxiumum number of characters that can be read (IE the size of the output buffer)
//
// RETURNS       :
//   Number of characters successfully queued (0 if none are read)

unsigned int getFromCircularQueue(char* ptrOutputBuffer, unsigned int bufferLength)
{
	unsigned int readCount;								
	unsigned int count = 0;													//variable to store counter value
	// add code to implement the removing from the circular queue using the data structures in myQueueStruct
	// and place the data into the ptrOutputBuffer up to a maximum number of bytes specified by bufferLength
	// return the count of the bytes that was dequeued
	
		for(readCount = 0 ; readCount <bufferLength;readCount++)			//for loop to keep readcount in the bufferlength limit
		{
			*ptrOutputBuffer = *myQueue.ptrCircularTail;					//copying the data of tail pointer into ptr output buffer
			myQueue.ptrCircularTail++;										//incrementing the tail pointer
			ptrOutputBuffer++;												//incrementing the output buffer by 1 
			
			count++;														//increasing the counter variable by 1
			if (count > myQueue.queueCount)									// checking if tail pointer exceeds the count or not
			{
				myQueue.ptrCircularTail = myQueue.ptrBuffer;				//copying the pointer buffer into the tail.
			}
			
		}
	
		return readCount;
}

// FUNCTION      : myReadThreadFunction
// DESCRIPTION   :
//   A seperate thread from the main program that will independently check for data in queue and print it out
//   DO NOT CHANGE THIS CODE!
// PARAMETERS    :
//   lpParam - arguement passed into thread (not used in this example)
//
// RETURNS       :
//   Will never return so no effective return value

DWORD WINAPI myReadThreadFunction(LPVOID lpParam)
{
	char readBuffer[BUFFER_SIZE];     // local buffer to put the data into when reading from queue and print
	unsigned int  readCount = 0;      // the number of characters read from queue

	while (1)
	{
		Sleep(INPUT_TIMEOUT_MS);      // wait for some data to be received

		// check if data is available and if so print it out
		readCount = getFromCircularQueue(readBuffer, BUFFER_SIZE);
		if (readCount != 0)           // check for reads that did not get any data
			printf("UNQUEUE: %s\n", readBuffer);
	}
	return 0;    // will never reach here
}