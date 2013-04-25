package com.sunwave;

import com.sunwave.jni.SWNativeEnCode;

import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.Menu;
import android.widget.TextView;

public class MainActivity extends Activity {
	
	private SWNativeEnCode mNativeEnCode = null;
	private SWSurfaceView mSurfaceView = null;
	private MyThread1 mThread1 = null;
	private MyThread2 mThread2 = null;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		mNativeEnCode = new SWNativeEnCode();
		mSurfaceView = (SWSurfaceView) findViewById(R.id.swsurfaceview);
		
		mThread1 = new MyThread1();
		mThread2 = new MyThread2();
		//mNativeEnCode.nativeInitEncode();
		byte buff[] = mNativeEnCode.nativeGetVideoBuffer();
		for (int i=0;i<buff.length;i++)
		{
			Log.i("aaaaa",""+buff[i]);
		}
		
		Log.i("aaaaa",""+buff.toString());
		//mThread1.start();
		//mThread2.start();
	}
	
	
	private class MyThread1 extends Thread{

		@Override
		public void run() {
			int num = 1;
			int d = 0;
			while(num++ <= 100) {
				
				d = num+5;
				Log.d("aaaaa", ""+d);
				mNativeEnCode.nativeSetRawBuffer(null, 0, d);
				try {
					sleep(100);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			
		}
		
		
	}
	
	private class MyThread2 extends Thread{

		@Override
		public void run() {
			int num = 0;
			while(num++ <= 100) {
				Log.d("bbbbb", ""+mNativeEnCode.nativeGetVideoBuffer());
				try {
					sleep(200);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
		
		
	}
	
	@Override
	protected void onDestroy() {
		super.onDestroy();
		if (mNativeEnCode != null) {
			mNativeEnCode.nativeStopEncode();
			mNativeEnCode.nativeReleaseEncode();
			mNativeEnCode = null;
		}
	}
	
}
