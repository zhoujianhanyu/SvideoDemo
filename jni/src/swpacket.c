#include "swpacket.h"
#include "swlog.h"

void *InitPacketQueue()
{
	SWPacketQueue *ppacketqueue = NULL;
	ppacketqueue = (SWPacketQueue *)malloc(sizeof(SWPacketQueue));

	if (ppacketqueue == NULL)
	{
		return NULL;
	}
	memset(ppacketqueue, 0, sizeof(SWPacketQueue));
	ppacketqueue->front = NULL;
	ppacketqueue->rear = NULL;
	ppacketqueue->size = 0;
	pthread_mutex_init(&ppacketqueue->mutex,NULL);
	pthread_cond_init(&ppacketqueue->cond,NULL);
	return ppacketqueue;
}

bool ReleasePacketQueue(void *packetqueue)
{
	SWPacketQueue *ppacketqueue = (SWPacketQueue *)packetqueue;
	
	if (ppacketqueue != NULL)
	{	
		pthread_mutex_lock(&ppacketqueue->mutex);
		
		while (ppacketqueue->front)
		{
			ppacketqueue->rear = ppacketqueue->front->next;
			free(ppacketqueue->front);
			ppacketqueue->front = NULL;
			ppacketqueue->front = ppacketqueue->rear;
  		}
  		
		pthread_mutex_unlock(&ppacketqueue->mutex);
		pthread_mutex_destroy(&ppacketqueue->mutex);
		pthread_cond_destroy(&ppacketqueue->cond);

		free(ppacketqueue);
	
	}
	
	return true;
}

bool PacketQueueEmpty(void *packetqueue)
{
	SWPacketQueue *ppacketqueue = (SWPacketQueue *)packetqueue;
	pthread_mutex_lock(&ppacketqueue->mutex);
	if (ppacketqueue && ppacketqueue->size != 0)
	{
		return false;
	}
	pthread_mutex_unlock(&ppacketqueue->mutex);
	return true;
}

bool PacketQueuePut(void *packetqueue, const AVPacket *packet, bool flag)
{
	SWPacketQueue *ppacketqueue = (SWPacketQueue *)packetqueue;
		
	if (!ppacketqueue)
	{
		return false;
	}
	SWPacket *ppacket = NULL;
	if (flag)
	{
		ppacket = (SWPacket *)malloc(sizeof(SWPacket));
		if (!ppacket)
		{
			return false;
		}
		memset(ppacket,0,sizeof(SWPacket));
		ppacket->size = sizeof(SWPacket) - 8;
		ppacket->stream_index = -1;
		ppacket->next = NULL;
		ppacket->datasize = 0;
	}
	else
	{
		ppacket = (SWPacket *)malloc(sizeof(SWPacket)+packet->size);
		
		if (!ppacket)
		{
			return false;
		}
		memset(ppacket,0,sizeof(SWPacket)+packet->size);
		memcpy(ppacket->data,packet->data, packet->size);
		ppacket->size = sizeof(SWPacket)+packet->size - 8;
		ppacket->stream_index = packet->stream_index;
		ppacket->flags = packet->flags;
		ppacket->pts = packet->pts;
		ppacket->dts = packet->dts;
		ppacket->datasize = packet->size;
		ppacket->next = NULL;
	}
	pthread_mutex_lock(&ppacketqueue->mutex);
	
	if (ppacketqueue->size == 0)
	{			
		ppacketqueue->front = ppacket;
		ppacketqueue->rear = ppacket;
	}
	else
	{
		ppacketqueue->rear->next = ppacket;
		ppacketqueue->rear = ppacket;
	}
		
	ppacketqueue->size++;
	pthread_mutex_unlock(&ppacketqueue->mutex);
	LOGD("packet size %d",ppacketqueue->size);
	return true;
	
}

void *PacketQueueGet(void *packetqueue)
{
	SWPacketQueue *ppacketqueue = (SWPacketQueue *)packetqueue;
	
	if (!ppacketqueue)
	{
		return NULL;
	}
	
	SWPacket *ppacket = NULL;
	pthread_mutex_lock(&ppacketqueue->mutex);
	
	if (ppacketqueue->size == 0)
	{	
		pthread_mutex_unlock(&ppacketqueue->mutex);
		return NULL;
	}
	else
	{	
		ppacketqueue->size--;
		ppacket = ppacketqueue->front;
		
		if (ppacketqueue->size == 0)
		{
			ppacketqueue->front = NULL;
			ppacketqueue->rear = NULL;
		}
		else
		{
			ppacketqueue->front = ppacketqueue->front->next;
		}
	}
	pthread_mutex_unlock(&ppacketqueue->mutex);
	return ppacket;
}
