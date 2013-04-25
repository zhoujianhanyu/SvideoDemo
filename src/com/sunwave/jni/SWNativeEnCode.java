package com.sunwave.jni;

public class SWNativeEnCode {
	
	public native boolean nativeInitEncode();
	public native boolean nativeStartEncode();
	public native boolean nativeStopEncode();
	public native boolean nativeReleaseEncode();
	public native byte[] nativeGetVideoBuffer();
	
	public native boolean nativeSetRawBuffer(byte buff[],long time,int index);
	
	static {
        System.loadLibrary("swjni");
    }
}
