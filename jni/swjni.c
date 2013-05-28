#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <jni.h>
#include "swlog.h"
#include "swqueue.h"
#include "swencode.h"


static SWRawQueue *pSRawQueue = NULL;
static SWEnContext *pSEncodeCtx = NULL;

static JavaVM *g_jvm = NULL;
static jobject g_obj = NULL;


jboolean InitEncode(JNIEnv* env, jobject thiz, jint width, jint height, jobject obj)
{

	(*env)->GetJavaVM(env,&g_jvm);
	g_obj = (*env)->NewGlobalRef(env,obj);
	pSRawQueue = (SWRawQueue *)InitRawQueue();

	if (pSRawQueue == NULL)
	{
		return JNI_FALSE;
	}
	pSEncodeCtx = (SWEnContext *)InitEnContext((int)width, (int)height, pSRawQueue, g_jvm, g_obj);
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
	ReleaseEnContext(pSEncodeCtx);
	ReleaseRawQueue(pSRawQueue);
	pSEncodeCtx = NULL;
	pSRawQueue = NULL;
	return JNI_FALSE;
}

jboolean SetRawBuffer(JNIEnv* env, jobject thiz, jbyteArray barray,jint len, jlong time,jint index)
{
	if (len == -1)
	{
		RawQueuePut(pSRawQueue, NULL, (int)len, 0, 0);
	}
	else
	{
		char* data = (char*)(*env)->GetByteArrayElements(env, barray, 0);
		if (data == NULL)
		{
			return JNI_FALSE;
		}
		//int len = (int)(*env)->GetArrayLength(env, barray);
		RawQueuePut(pSRawQueue, data, (int)len, (long)time, (int) index);
		(*env)->ReleaseByteArrayElements(env, barray, data, 0);
	}
	return JNI_FALSE;
}


static JNINativeMethod gMethods[] =
{
	{"nativeInitEncode", "(IILcom/sunwave/SWPacketList;)Z", (void*)InitEncode},
	{"nativeStartEncode", "()Z", (void*)StartEncode},
	{"nativeStopEncode", "()Z", (void*)StopEncode},
	{"nativeReleaseEncode", "()Z", (void*)ReleaseEncode},
	{"nativeSetRawBuf", "([BIJI)Z", (void*)SetRawBuffer}

};

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

static int registerNatives(JNIEnv* env)
{
	const char* kClassName = "com/sunwave/jni/SWNativeEnCode";
	return registerNativeMethods(env, kClassName, gMethods,
			sizeof(gMethods) / sizeof(gMethods[0]));
}

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
	{
		return -1;
	}
	result = JNI_VERSION_1_4;
	LOGD("------OnLoad------\n");
	return result;
}

void NI_OnUnLoad(JavaVM* vm, void* reserved)
{
	LOGD("------OnUnLoad------\n");
}
