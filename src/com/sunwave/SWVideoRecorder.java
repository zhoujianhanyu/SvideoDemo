package com.sunwave;

import com.sunwave.jni.SWNativeEnCode;
import android.content.Context;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.Toast;

public class SWVideoRecorder extends SurfaceView implements
		SurfaceHolder.Callback {

	private final String TAG = "SWSurfaceView";

	private boolean D = false;
	private SurfaceHolder mSurfaceHolder;
	private int mWidth, mHeight;
	private Camera mCamera;
	private boolean isPreview;
	private Context mContext;
	private SWNativeEnCode mNativeEnCode;
	private boolean Flag = false;
	private long time;
	private int sumtime;
	private long lasttime = 0, slasttime;
	private int framenum;

	@Override
	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {

		if (D) {
			Log.d(TAG, "heightMeasureSpec:" + widthMeasureSpec
					+ "heightMeasureSpec:" + heightMeasureSpec);
		}
		super.onMeasure(widthMeasureSpec, heightMeasureSpec);
	}

	public SWVideoRecorder(Context context, AttributeSet attrs) {
		super(context, attrs);
		if (D) {
			Log.d(TAG, "SWSurfaceView(Context context, AttributeSet attrs)");
		}
		mContext = context;
		this.mWidth = 800;
		this.mHeight = 480;
		this.mSurfaceHolder = this.getHolder();
		if (mSurfaceHolder != null) {
			mSurfaceHolder.addCallback(this);
			// mSurfaceHolder.setFixedSize(176, 144); // 预览大小設置
			mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
			this.setKeepScreenOn(true);
		}

		isPreview = false;
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		if (D) {
			Log.d(TAG, "surfaceChanged");
		}
		initCamera();
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {

		openCamera();
		if (D) {
			Log.d(TAG, "surfaceCreated");
		}
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {

		closeCamera();
		if (D) {
			Log.d(TAG, "surfaceDestroyed");
		}

	}

	private void openCamera() {
		if (!isPreview) {
			mCamera = Camera.open();// 开启摄像头（2.3版本后支持多摄像头,需传入参数）
		}
		try {
			Log.i(TAG, "SurfaceHolder.Callback：surface Created");
			mCamera.setPreviewDisplay(mSurfaceHolder);
		} catch (Exception ex) {
			if (null != mCamera) {
				mCamera.release();
				mCamera = null;
			}
			Log.i(TAG + "initCamera", ex.getMessage());
		}

	}

	public void initCamera() {
		if (isPreview && mCamera != null) {
			mCamera.stopPreview();
		}
		if (mCamera != null) {
			try {

				Camera.Parameters parameters = mCamera.getParameters();
				parameters.setPreviewSize(mWidth, mHeight);
				// parameters.setPreviewFpsRange(9,30);
				parameters.setPictureFormat(ImageFormat.NV21);
				// parameters.setPictureSize(mWidth, mHeight);
				mCamera.setParameters(parameters);// android2.3.3以后不需要此行代码
				mCamera.setPreviewCallback(new StreamIt());
				mCamera.startPreview();
				// mCamera.autoFocus(null);
			} catch (Exception e) {
				e.printStackTrace();
			}
			isPreview = true;
		}
	}

	public void closeCamera() {

		if (mSurfaceHolder != null) {
			mSurfaceHolder.removeCallback(this);
			mSurfaceHolder = null;
		}
		if (mCamera != null) {
			mCamera.setPreviewCallback(null); //！！这个必须在前，不然退出出错
			mCamera.stopPreview();
			isPreview = false;
			mCamera.release();
			mCamera = null;
		}
	}

	public void setFlag(boolean flag) {
		Flag = flag;
	}

	public void setmNativeEnCode(SWNativeEnCode mNativeEnCode) {
		this.mNativeEnCode = mNativeEnCode;
	}
	int num = 0;
	class StreamIt implements Camera.PreviewCallback {

		@Override
		public void onPreviewFrame(byte[] data, Camera camera) {
			// Size size = camera.getParameters().getPreviewSize();

			if (Flag) {
				num++;
				if (num > 100)
				{	
					mNativeEnCode.nativeSetRawBuf(null, -1, 0, 0);
					Flag = false;
					return;
				}
				if (lasttime == 0) {
					time = 0;
					lasttime = System.currentTimeMillis();
				} else {

					time = System.currentTimeMillis() - lasttime;
				}
				/*Log.d(TAG, "" + time);
				framenum++;
				sumtime = (int) (time - slasttime);
				if (sumtime > 5000) {
					slasttime = time;
					
				}*/
				mNativeEnCode.nativeSetRawBuf(data, data.length, time, 0);
			}
		}
	}
	
	private void frameSpeed(byte[] data,float k){
		if (data == null){
			framenum = 0;
			lasttime = 0;
			mNativeEnCode.nativeSetRawBuf(data, -1, 0, 0);
		}
		if (lasttime == 0) {
			time = 0;
			lasttime = System.currentTimeMillis();
		} else {

			time = System.currentTimeMillis() - lasttime;
		}
		framenum++;
		sumtime = (int) (time - slasttime);
		if (sumtime > 5000) {
			float speed = (float) ((framenum*1.0)/(sumtime*1.0));
			if(speed <= k)
			{
				mNativeEnCode.nativeSetRawBuf(data, data.length, time, 0);
			}
			slasttime = time;
			framenum = 0;
		}
		
	}
}
