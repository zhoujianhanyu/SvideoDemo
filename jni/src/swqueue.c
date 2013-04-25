#include "swqueue.h"

bool quit = false;

void *InitRawQueue()
{
	SWRawQueue *prawqueue = NULL;
	prawqueue = (SWRawQueue *)malloc(sizeof(SWRawQueue));
	
	if (prawqueue == NULL)
	{
		return NULL;
	}
	memset(prawqueue, 0, sizeof(SWRawQueue));
	prawqueue->front = NULL;
	prawqueue->rear = NULL;
	prawqueue->size = 0;
	quit = false;
	return prawqueue;
}

bool ReleaseRawQueue(void *rawqueue)
{
	SWRawQueue *prawqueue = (SWRawQueue *)rawqueue;
	
	if (prawqueue != NULL)
	{
		quit = true;
		pthread_mutex_lock(&prawqueue->mutex);
		
		while (prawqueue->front) 			
		{
			prawqueue->rear = prawqueue->front->next;
			free(prawqueue->front);
			prawqueue->front = NULL;
			prawqueue->front = prawqueue->rear;
  		}
  		
	pthread_mutex_unlock(&prawqueue->mutex);
	free(prawqueue);
	
	}
	
	return true;
}

bool RawQueueEmpty(void *rawqueue)
{	
	SWRawQueue *prawqueue = (SWRawQueue *)rawqueue;
	
	if (prawqueue && prawqueue->size != 0)
	{
		return false;
	}
	
	return true;
}

bool RawQueuePut(void *rawqueue, void *buffer, int len, short stream_index, long time)
{
	SWRawQueue *prawqueue = (SWRawQueue *)rawqueue;
	
	if (!prawqueue)
	{
		return false;
	}
	SWNode *pnode = NULL;
	if (len == -1)
	{
		pnode = (SWNode *)malloc(sizeof(SWNode));
		if (!pnode)
		{
			return false;
		}

		memset(pnode,0,sizeof(SWNode));
		pnode->stream_index = -1;
		pnode->next = NULL;
	}
	else 
	{
		if (!buffer || len <= 0)
		{
			return false;
		}
		pnode = (SWNode *)malloc(sizeof(SWNode)+len);
		
		if (!pnode)
		{
			return false;
		}

		memset(pnode,0,sizeof(SWNode)+len);		
		pnode->time = time;
		pnode->stream_index = stream_index;			
		memcpy(pnode->data, buffer, len);
		pnode->next = NULL;
	}
	pthread_mutex_lock(&prawqueue->mutex);
		
	if (prawqueue->size == 0)
	{	
			
		prawqueue->front = pnode;
		prawqueue->rear = pnode;
	}
	else
	{
		prawqueue->rear->next = pnode;
		prawqueue->rear = pnode;		
	}
		
	prawqueue->size++;
	pthread_mutex_unlock(&prawqueue->mutex);
	return true;

}

void *RawQueueGet(void *rawqueue)
{
	SWRawQueue *prawqueue = (SWRawQueue *)rawqueue;
	
	if (!prawqueue || quit)
	{
		return NULL;
	}
	
	SWNode *pnode = NULL;
	pthread_mutex_lock(&prawqueue->mutex);
	
	if (prawqueue->size == 0)
	{	
		pthread_mutex_unlock(&prawqueue->mutex);
		return NULL;
	}
	else
	{	
		prawqueue->size--;
		pnode = prawqueue->front;
		
		if (prawqueue->size == 0)
		{
			prawqueue->front = NULL;
			prawqueue->rear = NULL;
		}
		else
		{
			prawqueue->front = prawqueue->front->next;
		}
	}
	
	pthread_mutex_unlock(&prawqueue->mutex);
	return pnode;

}
