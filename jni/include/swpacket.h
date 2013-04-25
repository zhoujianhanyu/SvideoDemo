#ifndef _SWPACKET_H
#define _SWPACKET_H


#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include    <pthread.h>
#include    <stdbool.h>

typedef struct _SWPacket
{
	short stream_index;
	int pts;
	int dts;
	bool key;
    int size;
    struct _SWPacket *next;
    char data[0];
    
}__attribute__((packed)) SWPacket;

typedef struct _SWPacketQueue
{
	SWPacket *front;
	SWPacket *rear;
	int size;
	pthread_mutex_t mutex;
  	pthread_cond_t  cond;
  	
}SWPacketQueue;

void *InitPacketQueue();
bool ReleasePacketQueue(void *packetqueue);
bool PacketQueueEmpty(void *packetqueue);
bool PacketQueuePut(void *packetqueue, void *data,int len, bool key, short stream_index, int pts, int dts);
void *PacketQueueGet(void *packetqueue);

#endif /* _SWPACKET_H */
