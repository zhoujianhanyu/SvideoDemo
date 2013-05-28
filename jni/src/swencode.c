#include   "swencode.h"
#include   "swqueue.h"
#include   "swlog.h"

#define SW_TYPE_VIDEO 0
#define SW_TYPE_AUDIO 1
#define SW_VIDEO_CODEC_ID AV_CODEC_ID_MPEG1VIDEO
#define SW_AUDIO_CODEC_ID AV_CODEC_ID_AAC

bool InitFrame();
bool ReleaseFrame();

static bool quit = false;
static bool running = false;

static JavaVM *g_jvm = NULL;
static jobject g_obj = NULL;

static SWRawQueue *pERawQueue = NULL;
static AVFrame *pFrameVideo = NULL;
static AVFrame *pFrame = NULL;
static AVFrame *pFrameAudio = NULL;
static uint8_t *pBufferVideo = NULL;
static int numBytesVideo;

static char tembuf[6000];
static int peachsize[30];
static int bufsize = 0;
static int bufnum = 0;
typedef struct _UDPpacket {
	//int size;
	int stream_index;
	int stream_num;
	int size1;
	int size2;
	int size3;
	int size4;
	int size5;
	int datasize;
	char data[0];

}__attribute__((packed)) UDPpacket;

typedef struct _Packet {

	int size;
	int64_t pts;
	int64_t dts;
	int flags;
	int datasize;
	char data[0];

}__attribute__((packed)) Packet;

bool JCallBackFun(JNIEnv *env, jmethodID mid, const AVPacket *pkt) {
	UDPpacket *upacket = NULL;
	Packet *packet = NULL;
	jbyteArray barr = NULL;
	int isover = 1;
	int len = 0;
	if (pkt == NULL) {
		isover = -1;
		if (bufsize > 0) {
			len = sizeof(UDPpacket) + bufsize;
			upacket = (UDPpacket *) malloc(len);
			if (upacket == NULL) {
				(*env)->CallVoidMethod(env, g_obj, mid, barr, 0, isover);
				return false;
			}

			upacket->size1 = peachsize[0];
			upacket->size2 = peachsize[1];
			upacket->size3 = peachsize[2];
			upacket->size4 = peachsize[3];
			upacket->size5 = peachsize[4];
			upacket->stream_index = SW_TYPE_AUDIO;
			upacket->stream_num = bufnum;
			upacket->datasize = bufsize;
			memcpy(upacket->data, tembuf, bufsize);
			barr = (*env)->NewByteArray(env, len);
			if (barr != NULL) {
				(*env)->SetByteArrayRegion(env, barr, 0, len,
						(jbyte *) upacket);
			} else {
				len = -1;
			}
		}
		bufnum = 0;
		bufsize = 0;
	} else {
		len = sizeof(Packet) + pkt->size;
		LOGD("----%d---%d", len, bufnum);
		packet = (Packet *) malloc(len);
		if (packet == NULL) {
			return false;
		}
		memset(packet, 0, len);
		//upacket->size = len;
		//upacket->stream_index = packet->stream_index;
		packet->size = len;
		packet->pts = pkt->pts;
		packet->dts = pkt->dts;
		packet->flags = pkt->flags;
		packet->datasize = pkt->size;
		memcpy(packet->data, pkt->data, pkt->size);
		if (pkt->stream_index == SW_TYPE_VIDEO) {
			len = sizeof(UDPpacket) + packet->size;
			upacket = (UDPpacket *) malloc(len);
			if (upacket == NULL) {
				free(packet);
				return false;
			}
			//upacket->size = len;
			upacket->size1 = 0;
			upacket->size2 = 0;
			upacket->size3 = 0;
			upacket->size4 = 0;
			upacket->size5 = 0;
			upacket->stream_index = SW_TYPE_VIDEO;
			upacket->stream_num = 1;
			upacket->datasize = packet->size;
			memcpy(upacket->data, packet, packet->size);
			barr = (*env)->NewByteArray(env, len);
			if (barr != NULL) {
				(*env)->SetByteArrayRegion(env, barr, 0, len,
						(jbyte *) upacket);
			} else {
				len = -1;
			}
		} else {
			peachsize[bufnum] = packet->size;
			bufnum++;
			memcpy(tembuf + bufsize, packet, packet->size);
			bufsize += packet->size;
			if (bufnum < 5) {
				free(packet);
				packet = NULL;
				return true;
			}
			len = sizeof(UDPpacket) + bufsize;
			upacket = (UDPpacket *) malloc(len);
			if (upacket == NULL) {
				return false;
			}

			upacket->size1 = peachsize[0];
			upacket->size2 = peachsize[1];
			upacket->size3 = peachsize[2];
			upacket->size4 = peachsize[3];
			upacket->size5 = peachsize[4];

			upacket->stream_index = SW_TYPE_AUDIO;
			upacket->stream_num = bufnum;
			upacket->datasize = bufsize;
			memcpy(upacket->data, tembuf, bufsize);
			barr = (*env)->NewByteArray(env, len);
			if (barr != NULL) {
				bufsize = 0;
				bufnum = 0;
				(*env)->SetByteArrayRegion(env, barr, 0, len,
						(jbyte *) upacket);
			} else {
				len = -1;
			}

		}
	}
	(*env)->CallVoidMethod(env, g_obj, mid, barr, len, isover);
	(*env)->DeleteLocalRef(env, barr);
	if (packet != NULL) {
		free(packet);
		packet = NULL;
	}
	if (upacket != NULL) {
		free(upacket);
		upacket = NULL;
	}
	return true;
}

void *InitEnContext(int width, int height, void *rawqueue, JavaVM *jvm,
		jobject obj) {

	if (!rawqueue) {
		return NULL;
	}

	pERawQueue = rawqueue;
	SWEnContext *pencontext = NULL;
	AVCodec *pCodec = NULL, *aCodec = NULL;
	pencontext = (SWEnContext *) malloc(sizeof(SWEnContext));

	if (pencontext == NULL) {
		return NULL;
	}

	quit = false;
	running = false;

	avcodec_register_all();
	av_register_all();
	pCodec = avcodec_find_encoder(SW_VIDEO_CODEC_ID);

	if (pCodec == NULL) {
		free(pencontext);
		pencontext = NULL;
		return NULL;
	}

	aCodec = avcodec_find_encoder(SW_AUDIO_CODEC_ID);

	if (aCodec == NULL) {
		free(pencontext);
		pencontext = NULL;
		return NULL;
	}

	pencontext->pVCodecCtx = avcodec_alloc_context3(pCodec);
	pencontext->pACodecCtx = avcodec_alloc_context3(aCodec);

	if (!pencontext->pVCodecCtx || !pencontext->pACodecCtx) {
		free(pencontext);
		pencontext = NULL;
		return NULL;
	}

	if (pCodec->capabilities & CODEC_CAP_TRUNCATED) {
		pencontext->pVCodecCtx->flags |= CODEC_FLAG_TRUNCATED;
	}

	pencontext->pVCodecCtx->codec_id = SW_VIDEO_CODEC_ID;
	pencontext->pVCodecCtx->width = width;
	pencontext->pVCodecCtx->height = height;
	pencontext->pVCodecCtx->bit_rate = 400000;
	pencontext->pVCodecCtx->time_base = (AVRational ) { 1, 25 };
	pencontext->pVCodecCtx->gop_size = 10; /* emit one intra frame every ten frames */
	pencontext->pVCodecCtx->max_b_frames = 1;
	pencontext->pVCodecCtx->pix_fmt = PIX_FMT_YUV420P;

	pencontext->pSwsCtx = sws_getContext(pencontext->pVCodecCtx->width,
			pencontext->pVCodecCtx->height, PIX_FMT_NV21,
			pencontext->pVCodecCtx->width, pencontext->pVCodecCtx->height,
			pencontext->pVCodecCtx->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);

	if (!pencontext->pSwsCtx
			|| avcodec_open2(pencontext->pVCodecCtx, pCodec, NULL) < 0) {
		avcodec_close(pencontext->pVCodecCtx);
		free(pencontext);
		pencontext = NULL;
		return NULL;
	}
	pencontext->pACodecCtx->codec_id = SW_AUDIO_CODEC_ID;
	pencontext->pACodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
	pencontext->pACodecCtx->sample_rate = 44100;
	pencontext->pACodecCtx->bit_rate = 64000;
	pencontext->pACodecCtx->channels = 1;
	if (avcodec_open2(pencontext->pACodecCtx, aCodec, NULL) < 0) {
		avcodec_close(pencontext->pVCodecCtx);
		avcodec_close(pencontext->pACodecCtx);
		free(pencontext);
		pencontext = NULL;
		return NULL;
	}
	g_jvm = jvm;
	g_obj = obj;
	return pencontext;
}

void *EnContextThread(void *encontext) {

	if (!pERawQueue || !encontext) {
		return NULL;
	}
	SWEnContext *pencontext = (SWEnContext *) encontext;
	if (!InitFrame(pencontext)) {
		return NULL;
	}
	JNIEnv *env;
	jclass cls;
	jmethodID mid;

	if ((g_jvm != NULL)
			&& (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL) != JNI_OK) {
		LOGD("%s: AttachCurrentThread() failed", __FUNCTION__);
		return NULL;
	}
	if (g_obj != NULL) {
		cls = (*env)->GetObjectClass(env, g_obj);
	}
	if (cls == NULL) {
		return NULL;
	}
	mid = (*env)->GetMethodID(env, cls, "setPacket", "([BII)Z");
	bufsize = 0;
	bufnum = 0;
	quit = false;
	running = true;
	int video_output = 1;
	int audio_output = 1;

	SWNode *node = NULL;

	AVPacket vpkt;
	AVPacket apkt;
	int ret;
	int num = 0;

	while (!quit) {
		node = (SWNode *) RawQueueGet(pERawQueue);
		if (!node) {
			usleep(200000);
			continue;
		}
		if (node->stream_index == SW_TYPE_VIDEO) {

			avpicture_fill((AVPicture *) pFrame, (uint8_t *) node->data,
					PIX_FMT_NV21, pencontext->pVCodecCtx->width,
					pencontext->pVCodecCtx->height);

			if (video_output) {
				av_init_packet(&vpkt);
				vpkt.data = NULL;
				vpkt.size = 0;
			}

			sws_scale(pencontext->pSwsCtx,
					(const uint8_t* const *) pFrame->data, pFrame->linesize, 0,
					pencontext->pVCodecCtx->height, pFrameVideo->data,
					pFrameVideo->linesize);

			pFrameVideo->pts = (int64_t) node->time/50;
			ret = avcodec_encode_video2(pencontext->pVCodecCtx, &vpkt,
					pFrameVideo, &video_output);

			if (ret < 0) {
				fprintf(stderr, "Error encoding frame\n");
				exit(1);
			}
			if (video_output) {

				if (pencontext->pVCodecCtx->coded_frame->key_frame) {
					vpkt.flags |= AV_PKT_FLAG_KEY;
				}
				vpkt.stream_index = SW_TYPE_VIDEO;

				JCallBackFun(env, mid, &vpkt);

				av_free_packet(&vpkt);

			}

		} else if (node->stream_index == SW_TYPE_AUDIO) {

			ret = avcodec_fill_audio_frame(pFrameAudio,
					pencontext->pACodecCtx->channels,
					pencontext->pACodecCtx->sample_fmt,
					(const uint8_t*) node->data, node->size, 1);
			if (ret < 0) {

				break;
			}
			//pFrameAudio->pts = (int64_t) node->time/25;

			if (audio_output) {
				av_init_packet(&apkt);
				apkt.data = NULL;
				apkt.size = 0;
			}

			ret = avcodec_encode_audio2(pencontext->pACodecCtx, &apkt,
					pFrameAudio, &audio_output);
			if (ret < 0) {

				break;
			}
			if (audio_output) {
				apkt.stream_index = SW_TYPE_AUDIO;
				apkt.flags |= AV_PKT_FLAG_KEY;
				JCallBackFun(env, mid, &apkt);
				av_free_packet(&apkt);
			}
		} else {
			break;
		}
		free(node);
		node = NULL;
	}
	if (node) {
		free(node);
		node = NULL;
	}

	JCallBackFun(env, mid, NULL);
	(*env)->DeleteLocalRef(env, cls);
	if ((*g_jvm)->DetachCurrentThread(g_jvm) != JNI_OK) {
		LOGD("%s: DetachCurrentThread() failed", __FUNCTION__);
	}
	avcodec_close(pencontext->pVCodecCtx);
	av_free(pencontext->pVCodecCtx);
	ReleaseFrame();
	running = false;
	pthread_exit(NULL);
	return NULL;
}

bool StartEnContext(void *encontext) {
	SWEnContext *pencontext = (SWEnContext *) encontext;

	if (!pencontext
			|| pthread_create(&pencontext->thread, NULL, &EnContextThread,
					pencontext) != 0) {

		return false;
	}
	return true;
}

bool ReleaseEnContext(void *encontext) {
	SWEnContext *pencontext = (SWEnContext *) encontext;

	if (!pencontext) {
		return false;
	}

	avcodec_close(pencontext->pVCodecCtx);
	avcodec_close(pencontext->pACodecCtx);
	free(pencontext);
	return true;
}

bool StopEnContext(void *encontext) {
	SWEnContext *pencontext = (SWEnContext *) encontext;

	if (!pencontext) {
		return false;
	}

	quit = true;
	pthread_join(pencontext->thread, NULL);
	while (running) {
		usleep(200000);
	}

	return true;
}

bool InitFrame(SWEnContext *pencontext) {
	pFrame = avcodec_alloc_frame();
	pFrameVideo = avcodec_alloc_frame();
	pFrameAudio = avcodec_alloc_frame();
	if (pFrame == NULL || pFrameVideo == NULL || pFrameAudio == NULL) {
		return false;
	}

	numBytesVideo = avpicture_get_size(PIX_FMT_YUV420P,
			pencontext->pVCodecCtx->width, pencontext->pVCodecCtx->height);

	pBufferVideo = av_malloc(numBytesVideo);

	if (!pBufferVideo) {
		ReleaseFrame();
		return false;
	}
	pFrameAudio->format = pencontext->pACodecCtx->sample_fmt;
	pFrameAudio->nb_samples = pencontext->pACodecCtx->frame_size;
	pFrameAudio->channel_layout = pencontext->pACodecCtx->channel_layout;

	avpicture_fill((AVPicture *) pFrameVideo, pBufferVideo, PIX_FMT_YUV420P,
			pencontext->pVCodecCtx->width, pencontext->pVCodecCtx->height);

	return true;
}

bool ReleaseFrame() {

	if (!pBufferVideo) {
		av_freep(&pBufferVideo);
		pBufferVideo = NULL;
	}
	if (!pFrame) {
		avcodec_free_frame(&pFrame);
		pFrame = NULL;
	}
	if (!pFrameVideo) {
		avcodec_free_frame(&pFrameVideo);
		pFrameVideo = NULL;
	}
	if (!pFrameAudio) {
		avcodec_free_frame(&pFrameAudio);
		pFrameAudio = NULL;
	}

	return true;
}

