#ifndef _SWPACKET_H
#define _SWPACKET_H


#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include    <pthread.h>
#include    <stdbool.h>
#include    "swpacket.h"
#include	"libavcodec/avcodec.h"
typedef struct _SWPacket
{
	struct _SWPacket *next;
	int size;
	int stream_index;
	int64_t pts;
	int64_t dts;
	int flags;
	int datasize;
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
bool PacketQueuePut(void *packetqueue, const AVPacket *packet,bool flag);
void *PacketQueueGet(void *packetqueue);

#endif /* _SWPACKET_H */
