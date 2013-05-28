package com.sunwave.jni;

import com.sunwave.SWPacketList;

public class SWNativeEnCode {
	
	public native boolean nativeInitEncode(int width, int height, SWPacketList packetlist);
	public native boolean nativeStartEncode();
	public native boolean nativeStopEncode();
	public native boolean nativeReleaseEncode();
	//public native byte[] nativeGetVideoBuffer();
	
	public native boolean nativeSetRawBuf(byte buff[],int len, long time,int index);
	
	static {
		
        System.loadLibrary("swjni");
    }
	
}
