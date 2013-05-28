

LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/faac/include \
	$(LOCAL_PATH)/x264/include \
	$(LOCAL_PATH)/ffmpeg/include \
	$(LOCAL_PATH)/include


#LOCAL_STATIC_LIBRARIES := libavcodec libavformat libavutil libswscale libx264 libfaac


LOCAL_LDLIBS += -L$(LOCAL_PATH)/ffmpeg/lib \
				-lavcodec -lavformat -lavutil -lswscale
				
LOCAL_LDLIBS += -L$(LOCAL_PATH)/faac/lib -lfaac
LOCAL_LDLIBS += -L$(LOCAL_PATH)/zlib/lib -lz					
LOCAL_LDLIBS += -L$(LOCAL_PATH)/x264/lib -lx264
LOCAL_LDLIBS +=  -llog 
						


LOCAL_MODULE		:= swjni
LOCAL_SRC_FILES		:= swjni.c \
					 src/swencode.c src/swqueue.c
					
					

include $(BUILD_SHARED_LIBRARY)
