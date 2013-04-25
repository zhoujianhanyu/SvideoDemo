#include   "swencode.h"
#include   "swqueue.h"
#include   "swpacket.h"

#define SW_TYPE_VIDEO 0
#define SW_TYPE_AUDIO 1

static bool quit = false;
static bool running = false;

SWRawQueue *pERawQueue = NULL;
SWPacketQueue *pEPacketQueue = NULL;

void *InitEnContext(void *rawqueue, void *packetQueue)
{
	if (!rawqueue || !packetQueue) 
   	{
   	 	return NULL;
   	}
   	
	pERawQueue = rawqueue;
	pEPacketQueue = packetQueue;
	SWEnContext *pencontext = NULL;
	AVCodec *pCodec = NULL, *aCodec = NULL;
	pencontext = (SWEnContext *)malloc(sizeof(SWEnContext));
	
	if (pencontext == NULL)
	{	
		return NULL;
	}

	quit = false;
	running = false;
	
	avcodec_register_all();
	av_register_all();
	pCodec = avcodec_find_encoder(AV_CODEC_ID_MPEG1VIDEO);
	if (!pCodec)
	{
		free(pencontext);
		pencontext = NULL;
        return NULL;
	}
	aCodec = avcodec_find_encoder(86016);  

	if (!aCodec) 
	{	
		free(pencontext);
		pencontext = NULL;
        return NULL;
    }

    pencontext->pVCodecCtx = avcodec_alloc_context3(pCodec);
    pencontext->pACodecCtx = avcodec_alloc_context3(aCodec);
    
    if (!pencontext->pVCodecCtx || !pencontext->pACodecCtx) 
    {
        free(pencontext);
		pencontext = NULL;
        return NULL;
    }

    if(pCodec->capabilities&CODEC_CAP_TRUNCATED)
    {
        pencontext->pVCodecCtx->flags|= CODEC_FLAG_TRUNCATED; 
    }
    
    pencontext->pVCodecCtx->width = 352;
    pencontext->pVCodecCtx->height = 288;
    pencontext->pVCodecCtx->bit_rate = 400000;
    pencontext->pVCodecCtx->time_base= (AVRational){1,25};
    pencontext->pVCodecCtx->gop_size = 10; /* emit one intra frame every ten frames */
    pencontext->pVCodecCtx->max_b_frames=1;
    pencontext->pVCodecCtx->pix_fmt = PIX_FMT_YUV420P;
	
	pencontext->pSwsCtx = sws_getContext(pencontext->pVCodecCtx->width, 
										pencontext->pVCodecCtx->height, 
										PIX_FMT_YUV420P,
										pencontext->pVCodecCtx->width, 
										pencontext->pVCodecCtx->height, 
										pencontext->pVCodecCtx->pix_fmt,
										SWS_BICUBIC, NULL, NULL, NULL);
	
    if (!pencontext->pSwsCtx || avcodec_open2(pencontext->pVCodecCtx, pCodec, NULL) < 0) 
    {	
        avcodec_close(pencontext->pVCodecCtx);
        free(pencontext);
		pencontext = NULL;
        return NULL;
    }

    pencontext->pACodecCtx->codec_id = 86016;
    pencontext->pACodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    pencontext->pACodecCtx->sample_rate = 44100;
    pencontext->pACodecCtx->bit_rate = 64000;
    pencontext->pACodecCtx->channels   = 2;
   
    if (avcodec_open2(pencontext->pACodecCtx, aCodec, NULL) < 0)
    {	
    	avcodec_close(pencontext->pVCodecCtx);
    	avcodec_close(pencontext->pACodecCtx);
        free(pencontext);
		pencontext = NULL;
        return NULL;
    }
     
    return pencontext;  
}

void *EnContextThread(void *encontext)
{
	if (!pERawQueue || !pEPacketQueue || !encontext)
	{
		return false;
	}
	
	SWEnContext *pencontext = (SWEnContext *)encontext;      
	quit = false;
	running = true;
	int video_output = 1;
	int audio_output = 1;
	
	SWNode *node = NULL;
	AVFrame *frame = NULL;
	AVPacket vpkt;
	AVPacket apkt;
	int ret;
	bool key = false;
	//uint64_t video_frame_index = 0;
	
	while(!quit)
	{	
		node = (SWNode *)RawQueueGet(pERawQueue);
		if (!node)
		{	
			usleep(10);
			continue;
		}
		frame = (AVFrame *)node->data;
		
		if (node->stream_index == SW_TYPE_VIDEO)
		{	
			if (video_output)
			{
				av_init_packet(&vpkt);
				vpkt.data = NULL;
				vpkt.size = 0;
			}
			
			//frame->pts = av_rescale(video_frame_index,AV_TIME_BASE*(int64_t)
			//						pSWContext->pVCodecCtx->time_base.num,
			//						pSWContext->pVCodecCtx->time_base.den);
			ret = avcodec_encode_video2(pencontext->pVCodecCtx, &vpkt, frame, &video_output);

			if (ret < 0) 
			{
				fprintf(stderr, "Error encoding frame\n");
				exit(1);
			}
			if (video_output) 
			{	
				key = false;
				if (pencontext->pVCodecCtx->coded_frame->key_frame)
				{
				    //vpkt.flags |= AV_PKT_FLAG_KEY;
				    key = true;   
				}
				//vpkt.stream_index = 0;
				PacketQueuePut(pEPacketQueue, (void *)vpkt.data,vpkt.size,key,
							  SW_TYPE_VIDEO,vpkt.pts,vpkt.dts);
				av_free_packet(&vpkt);	
			}
			//video_frame_index++;
		}			
		else if (node->stream_index == SW_TYPE_AUDIO)
		{	
			if (audio_output)
			{
				av_init_packet(&apkt);
				apkt.data = NULL;
				apkt.size = 0;
			}
			
			ret = avcodec_encode_audio2(pencontext->pACodecCtx, &apkt, frame, &audio_output);
			if (ret < 0) 
			{
				fprintf(stderr, "Error encoding frame\n");
				exit(1);
			}
			if (audio_output) 
			{	
				//apkt.stream_index = 1;
				PacketQueuePut(pEPacketQueue, (void *)apkt.data,apkt.size,false,
							  SW_TYPE_AUDIO,apkt.pts,apkt.dts);
				av_free_packet(&apkt);			
			}
		}
		else
		{
			break;
		}
		
    	free(node);
    	frame = NULL;
    	node = NULL;
    }
    
    if (node)
    {
		free(node);
		frame = NULL;
		node = NULL;
    }
    PacketQueuePut(pEPacketQueue, NULL,-1,false,0,0,0);  
    avcodec_close(pencontext->pVCodecCtx);
    av_free(pencontext->pVCodecCtx);	
	running = false;
	return NULL;
}


bool StartEnContext(void *encontext)
{	
	SWEnContext *pencontext = (SWEnContext *)encontext;

	if (!pencontext || pthread_create(&pencontext->thread, NULL, 
									  &EnContextThread, pencontext) != 0)
	{	
		return false;
	}
	
	return true;
}

bool ReleaseEnContext(void *encontext)
{	
	SWEnContext *pencontext = (SWEnContext *)encontext;
	
	if (!pencontext)
	{
		return false;
	}
		
	avcodec_close(pencontext->pVCodecCtx);
    avcodec_close(pencontext->pACodecCtx);
    free(pencontext);
    return true;
}

bool StopEnContext(void *encontext)
{
	SWEnContext *pencontext = (SWEnContext *)encontext;
	
	if (!pencontext)
	{
		return false;
	}
	
	quit = true;
	pthread_join(pencontext->thread, NULL);
	while (running)
	{
		sleep(200);
	}

	return true;	
}


