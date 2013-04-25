

LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/ffmpeg/include \
	$(LOCAL_PATH)/include

LOCAL_STATIC_LIBRARIES := libavcodec libavformat libavutil libswscale libz

LOCAL_LDLIBS :=  -llog

LOCAL_LDLIBS += $(LOCAL_PATH)/ffmpeg/lib/libavcodec.a
LOCAL_LDLIBS += $(LOCAL_PATH)/ffmpeg/lib/libavformat.a
LOCAL_LDLIBS += $(LOCAL_PATH)/ffmpeg/lib/libavutil.a
LOCAL_LDLIBS += $(LOCAL_PATH)/ffmpeg/lib/libswscale.a
LOCAL_LDLIBS += $(LOCAL_PATH)/zlib/lib/libz.a

LOCAL_MODULE		:=swjni
LOCAL_SRC_FILES		:=swjni.c \
					src/swqueue.c src/swpacket.c src/swencode.c
					

include $(BUILD_SHARED_LIBRARY)