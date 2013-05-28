#ifndef _SWLOG_H
#define _SWLOG_H


#include <android/log.h>


#define LOG_TAG "SWFFMPEG"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

#endif /* _SWLOG_H */
