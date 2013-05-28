#ifndef _SWENCODEC_H
#define _SWENCODEC_H


#include	<stdlib.h>
#include	<stdio.h>
#include    <string.h>
#include    <jni.h>
#include 	<assert.h>

#include	"libavformat/avformat.h"
#include	"libavcodec/avcodec.h"
#include	"libswscale/swscale.h"
#include    <pthread.h>
#include    <stdbool.h>

typedef struct _SWEnContext
{	

	AVCodecContext *pVCodecCtx;
	AVCodecContext *pACodecCtx;
	AVFrame *pFrame;
	pthread_t thread;
	struct SwsContext *pSwsCtx;
	
}SWEnContext;

void *InitEnContext(int width, int height, void *queue, JavaVM *jvm, jobject obj);
//void *InitEnContext(int width, int height, void *queue, void *packetqueue);

void *EnContextThread(void *encontext);

bool StartEnContext(void *encontext);
bool StopEnContext(void *encontext);
bool ReleaseEnContext(void *encontext);

#endif /* _SWENCODEC_H */

