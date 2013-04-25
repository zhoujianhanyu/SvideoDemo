#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <assert.h>
#include <android/log.h>

#include "swqueue.h"
#include "swpacket.h"
#include "swencode.h"

#define LOG_TAG "SWFFMPEG"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)


SWPacketQueue *pSPacketQueue = NULL;
SWRawQueue *pSRawQueue = NULL;
SWEnContext *pSEncodeCtx = NULL;

jboolean InitEncode(JNIEnv* env, jobject thiz)
{
	pSPacketQueue = (SWPacketQueue *)InitPacketQueue();
	if (pSPacketQueue == NULL)
	{
		return JNI_FALSE;
	}
	pSRawQueue = (SWRawQueue *)InitRawQueue();
	if (pSRawQueue == NULL)
	{
		return JNI_FALSE;
	}
	pSEncodeCtx = (SWEnContext *)InitEnContext(pSRawQueue, pSPacketQueue);
	if (pSEncodeCtx == NULL)
	{
		return JNI_FALSE;
	}
	return JNI_TRUE;
}

jboolean StartEncode(JNIEnv* env, jobject thiz)
{
	if (StartEnContext(pSEncodeCtx))
	{
		return JNI_TRUE;
	}
	return JNI_FALSE;
}

jboolean StopEncode(JNIEnv* env, jobject thiz)
{
	if (StopEnContext(pSEncodeCtx))
	{
		return JNI_TRUE;
	}
	return JNI_FALSE;
}


jboolean ReleaseEncode(JNIEnv* env, jobject thiz)
{
	if (ReleaseEnContext(pSEncodeCtx) &&
			ReleaseRawQueue(pSPacketQueue) &&
			ReleasePacketQueue(pSRawQueue))
	{
		pSEncodeCtx = NULL;
		pSPacketQueue = NULL;
		pSRawQueue = NULL;
		return JNI_TRUE;
	}
	return JNI_FALSE;
}

jbyteArray GetVideoBuffer(JNIEnv* env, jobject thiz)
{
	SWPacket *packet = (SWPacket *)PacketQueueGet(pSPacketQueue);
	if (packet == NULL)
	{
		return NULL;
	}
	int size = (int)packet->size;
	//int size = 10;
	jbyteArray array = (*env)->NewByteArray(env, size);
	(*env)->SetByteArrayRegion(env, array, 0, size, (jbyte*) packet);
	return array;
}

jboolean SetRawBuffer(JNIEnv* env, jobject thiz, jbyteArray barray,jlong time,jint index)
{
	int a = index;
	//(*env)->SetIntArrayRegion(env, array, size++, 1, &a);

	return JNI_FALSE;
}

jbyteArray aGetVideoBuffer(JNIEnv* env, jobject thiz)
{
	Test *test = (Test *)malloc(sizeof(Test)+10);
	memset(test,0,sizeof(Test)+10);
	test->kk = 10;
	test->size = sizeof(Test)+10;
	memcpy(test->bb,"abcdefgpoi",10);
	int size = (int)test->size;
	//int size = 10;
	jbyteArray array = (*env)->NewByteArray(env, size);
	(*env)->SetByteArrayRegion(env, array, 0, size, (jbyte*) test);
	return array;
}


/**
* 方法对应表
*/
static JNINativeMethod gMethods[] =
{
	{"nativeInitEncode", "()Z", (void*)InitEncode},
	{"nativeStartEncode", "()Z", (void*)StartEncode},
	{"nativeStopEncode", "()Z", (void*)StopEncode},
	{"nativeReleaseEncode", "()Z", (void*)ReleaseEncode},
	{"nativeGetVideoBuffer", "()[B", (void*)aGetVideoBuffer},
	{"nativeSetRawBuffer", "([BJI)Z", (void*)SetRawBuffer}

};

/*
* 为某一个类注册本地方法
*/
static int registerNativeMethods(JNIEnv* env
		, const char* className
		, JNINativeMethod* gMethods, int numMethods)
{
	jclass clazz;
	clazz = (*env)->FindClass(env, className);
	if (clazz == NULL)
	{
		return JNI_FALSE;
	}
	if ((*env)->RegisterNatives(env, clazz, gMethods, numMethods) < 0)
	{
		return JNI_FALSE;
	}

	return JNI_TRUE;
}


/*
* 为所有类注册本地方法
*/
static int registerNatives(JNIEnv* env)
{
	const char* kClassName = "com/sunwave/jni/SWNativeEnCode";//指定要注册的类
	return registerNativeMethods(env, kClassName, gMethods,
			sizeof(gMethods) / sizeof(gMethods[0]));
}

/*
* System.loadLibrary("lib")时调用
* 如果成功返回JNI版本, 失败返回-1
*/
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;

	if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK)
	{
		return -1;
	}
	assert(env != NULL);

	if (!registerNatives(env))
	{//注册
		return -1;
	}
	//成功
	result = JNI_VERSION_1_4;
	LOGD("------OnLoad------\n");
	return result;
}

void NI_OnUnLoad(JavaVM* vm, void* reserved)
{
	LOGD("------OnUnLoad------\n");
}
