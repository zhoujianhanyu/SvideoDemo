package com.sunwave;


import com.sunwave.jni.SWNativeEnCode;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.util.Log;

public class SWAudioRecorder implements Runnable {

	private static final String TAG = "SWAudioRecorder";
	private volatile boolean isRecording;
	private final Object mutex = new Object();
	private static final int frequency = 44100;
	private static final int audioEncoding = AudioFormat.ENCODING_PCM_16BIT;
	private SWNativeEnCode mNativeEnCode;
	private long time;
	private long lasttime;
	
	public SWAudioRecorder(SWNativeEnCode mNativeEnCode) {
		super();
		this.mNativeEnCode = mNativeEnCode;
	}

	public void run() {
		PcmWriter pcmWriter = new PcmWriter();
		pcmWriter.init();
		Thread writerThread = new Thread (pcmWriter);
		pcmWriter.setRecording(true);
		writerThread.start();
		synchronized (mutex) {
			while (!this.isRecording) {
				try {
					mutex.wait();
				} catch (InterruptedException e) {
					throw new IllegalStateException("Wait() interrupted!", e);
				}
			}
		}
		android.os.Process
				.setThreadPriority(android.os.Process.THREAD_PRIORITY_URGENT_AUDIO);

		int bufferRead = 0;
		int bufferSize = AudioRecord.getMinBufferSize(frequency,
				AudioFormat.CHANNEL_IN_MONO, audioEncoding);
		
		short[] tempBuffer = new short[bufferSize];
		AudioRecord recordInstance = new AudioRecord(
				MediaRecorder.AudioSource.MIC, frequency,
				AudioFormat.CHANNEL_IN_MONO, audioEncoding, bufferSize);

		recordInstance.startRecording();
		int num=0;
		while (this.isRecording) {
			if (num++ >800){
				break;
			}
			bufferRead = recordInstance.read(tempBuffer, 0, bufferSize);
			if (bufferRead == AudioRecord.ERROR_INVALID_OPERATION) {
				throw new IllegalStateException(
						"read() returned AudioRecord.ERROR_INVALID_OPERATION");
			} else if (bufferRead == AudioRecord.ERROR_BAD_VALUE) {
				throw new IllegalStateException(
						"read() returned AudioRecord.ERROR_BAD_VALUE");
			} else if (bufferRead == AudioRecord.ERROR_INVALID_OPERATION) {
				throw new IllegalStateException(
						"read() returned AudioRecord.ERROR_INVALID_OPERATION");
			}
			if (lasttime == 0) {
				time = 0;
				lasttime = System.currentTimeMillis();
			}else {
				
				time =  System.currentTimeMillis() - lasttime;
			}
			
			mNativeEnCode.nativeSetRawBuf(shortArrayToByteArray(tempBuffer,bufferRead), 2*bufferRead, time, 1);
			pcmWriter.putData(tempBuffer, bufferRead);
		}
		mNativeEnCode.nativeSetRawBuf(null, -1, 0, -1);
		recordInstance.stop();
		pcmWriter.setRecording(false);
	}
	
	public void setRecording(boolean isRecording) {
		synchronized (mutex) {
			this.isRecording = isRecording;
			if (this.isRecording) {
				mutex.notify();
			}
		}
	}

	public boolean isRecording() {
		synchronized (mutex) {
			return isRecording;
		}
	}
	
	public static byte[] shortArrayToByteArray(short[] s, int len) { 
		byte[] targets = new byte[2*len];
		for (int j = 0; j < len; j++){
			targets[2*j] = (byte) ((s[j]) & 0xff); 
			targets[2*j+1] = (byte) ((s[j] >>> 8) & 0xff); 
		}
		return targets; 
	}
}
