package com.sunwave;

import com.sunwave.jni.SWNativeEnCode;

import android.os.Bundle;
import android.app.Activity;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Menu;
import android.widget.TextView;

public class MainActivity extends Activity {

	private static final String TAG = "MainActivity";

	private SWNativeEnCode mNativeEnCode = null;
	private SWVideoRecorder mVideoRecorder = null;
	private SWPacketList mPacketList = null;
	// private SWSocketUDP mSocketUDP = null;
	private SWAudioRecorder mAudioRecorder = null;
	private SWSocketTCP mSocketTCP = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		mPacketList = new SWPacketList();
		mNativeEnCode = new SWNativeEnCode();
		mNativeEnCode.nativeInitEncode(800, 480, mPacketList);

		mVideoRecorder = (SWVideoRecorder) findViewById(R.id.swsurfaceview);
		mVideoRecorder.setmNativeEnCode(mNativeEnCode);
		
		mSocketTCP = new SWSocketTCP(mPacketList);
		startEnCode();
		
		mAudioRecorder = new SWAudioRecorder(mNativeEnCode);
		//Thread thread = new Thread(mAudioRecorder);
		//thread.start();
		//mNativeEnCode.nativeStartEncode();
		//mVideoRecorder.setFlag(true);
		//mAudioRecorder.setRecording(true);
	}


	private boolean startEnCode() {
		if (!mSocketTCP.openTCP()) {
			return false;
		}
		new Thread(mSocketTCP).start();
		mSocketTCP.setSending(true);
		
		mNativeEnCode.nativeStartEncode();
		mVideoRecorder.setFlag(true);
		return true;
	}

	private void stopEnCode() {

		mVideoRecorder.setFlag(false);
		mNativeEnCode.nativeStopEncode();
		mNativeEnCode.nativeReleaseEncode();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		if (mNativeEnCode != null) {
			mNativeEnCode.nativeStopEncode();
			mNativeEnCode.nativeReleaseEncode();
			mNativeEnCode = null;
		}
		if (mPacketList != null) {
			mPacketList.clearPacket();
			mPacketList = null;
		}
		if (mVideoRecorder != null) {
			mVideoRecorder.closeCamera();
			mVideoRecorder = null;
		}
		/*
		 * if (mSocketUDP != null) { mSocketUDP.closeSocket(); mSocketUDP =
		 * null; }
		 */
		if (mSocketTCP != null) {
			mSocketTCP.closeTCP();
			mSocketTCP = null;
		}
	}

}
