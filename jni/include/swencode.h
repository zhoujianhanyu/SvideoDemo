#ifndef _SWENCODEC_H
#define _SWENCODEC_H


#include	<stdlib.h>
#include	<stdio.h>
#include	"libavformat/avformat.h"
#include	"libavcodec/avcodec.h"
#include	"libswscale/swscale.h"
#include    <pthread.h>
#include    <stdbool.h>

typedef struct _test
{
	int kk;
	int size;
	char bb[0];
}__attribute__((packed)) Test;

typedef struct _SWEnContext
{	

	AVCodecContext *pVCodecCtx;
	AVCodecContext *pACodecCtx;

	AVFrame *pFrame;
	pthread_t thread;
	struct SwsContext *pSwsCtx;
	
}SWEnContext;

void *InitEnContext(void *queue, void *packetlink);

void *EnContextThread(void *encontext);

bool StartEnContext(void *encontext);
bool StopEnContext(void *encontext);
bool ReleaseEnContext(void *encontext);

#endif /* _SWENCODEC_H */

