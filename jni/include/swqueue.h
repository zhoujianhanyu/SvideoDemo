#ifndef _SWQUEUE_H
#define _SWQUEUE_H


#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include    <pthread.h>
#include    <stdbool.h>

typedef struct _SWNode
{
	long time;    
    short stream_index;
    int size;
    struct _SWNode *next;
    char data[0];
    
}SWNode;

typedef struct _SWRawQueue
{
	SWNode *front;
	SWNode *rear;
	int size;
	pthread_mutex_t mutex;
  	pthread_cond_t  cond;
  	
}SWRawQueue;

void *InitRawQueue();
bool ReleaseRawQueue(void *rawqueue);
bool RawQueueEmpty(void *rawqueue);
bool RawQueuePut(void *rawqueue, void *buffer, int len, long time, int stream_index);
void *RawQueueGet(void *rawqueue);

#endif /* _SWQUEUE_H */
