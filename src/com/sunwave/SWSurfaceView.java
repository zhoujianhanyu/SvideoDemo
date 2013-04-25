package com.sunwave;

import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.YuvImage;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class SWSurfaceView extends SurfaceView implements SurfaceHolder.Callback{

	private final String TAG =  this.getClass().getName();
	
	private boolean D = true;
	private SurfaceHolder mSurfaceHolder;
	private int mWidth, mHeight;
	private Camera mCamera; 
	private boolean isPreview;
	
	@Override
	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		
		if (D) {
			Log.d(TAG, "heightMeasureSpec:"+widthMeasureSpec+"heightMeasureSpec:"+heightMeasureSpec);
		}
		super.onMeasure(widthMeasureSpec, heightMeasureSpec);
	}

	public SWSurfaceView(Context context, AttributeSet attrs) {
		super(context, attrs);
		if (D) {
			Log.d(TAG, "SWSurfaceView(Context context, AttributeSet attrs)");
		}
		this.mWidth = 800;
		this.mHeight = 480;
		this.mSurfaceHolder = this.getHolder();
		if (mSurfaceHolder != null)
		{	
			mSurfaceHolder.addCallback(this);
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
		// 如果camera不为null ,释放摄像头
		closeCamera();
		if (D) {
			Log.d(TAG, "surfaceDestroyed");
		}
		
	}
	
	private void openCamera() {
    	if (!isPreview) {
			mCamera = Camera.open();
		}
		if (mCamera != null && !isPreview) {
			try{
				Camera.Parameters parameters = mCamera.getParameters();				
				parameters.setPreviewSize(mWidth, mHeight);    // 设置预览照片的大小				
				//parameters.setPreviewFpsRange(20,30);                    // 每秒显示20~30帧				
				parameters.setPictureFormat(ImageFormat.NV21);           // 设置图片格式				
				parameters.setPictureSize(mWidth, mHeight);    // 设置照片的大小
				//camera.setParameters(parameters);                      // android2.3.3以后不需要此行代码
				mCamera.setPreviewDisplay(mSurfaceHolder);                 // 通过SurfaceView显示取景画面				
		        mCamera.setPreviewCallback(new StreamIt());         // 设置回调的类				
				mCamera.startPreview();                                   // 开始预览				
				//mCamera.autoFocus(null);                                  // 自动对焦
			} catch (Exception e) {
				e.printStackTrace();
			}
			isPreview = true;
		}
    }
	private void closeCamera() {
		
		/*if (mSurfaceHolder != null) {
			mSurfaceHolder.removeCallback(this);
			mSurfaceHolder = null;
		}*/
		if (mCamera != null) {
			if (isPreview) {
				mCamera.stopPreview();
			}
			mCamera.release();
			mCamera = null;
		}
	}
	
	class StreamIt implements Camera.PreviewCallback {
				
	    @Override
	    public void onPreviewFrame(byte[] data, Camera camera) {
	        Size size = camera.getParameters().getPreviewSize();
	        if (D) {
	        	Log.d(TAG, ""+data.length);
	        }
	        /*try{ 
	        	//调用image.compressToJpeg（）将YUV格式图像数据data转为jpg格式
	            YuvImage image = new YuvImage(data, ImageFormat.NV21, size.width, size.height, null);  
	            if(image!=null){
	            	ByteArrayOutputStream outstream = new ByteArrayOutputStream();
	                image.compressToJpeg(new Rect(0, 0, size.width, size.height), 80, outstream); 
	                outstream.flush();
	                //启用线程将图像数据发送出去
	                Thread th = new MyThread(outstream,ipname);
	                th.start();              
	            }  
	        }catch(Exception ex){  
	            Log.e("Sys","Error:"+ex.getMessage());  
	        }        */
	    }
	}

}
