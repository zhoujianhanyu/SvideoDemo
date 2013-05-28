package com.sunwave;

import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;

import android.util.Log;

import com.sunwave.SWPacketList.SWPacket;

public class SWSocketTCP implements Runnable {
	
	private static final String TAG = "SWSocketTCP";
	private static final String ADD = "192.168.200.103";
	private static final int PORT = 8000;

	private volatile boolean isSending;
	private final Object mutex = new Object();
	private SWPacketList mPacketList = null;
	private Socket socket = null;
	private OutputStream os = null;
	
	public SWSocketTCP(SWPacketList mPacketList) {
		super();
		this.mPacketList = mPacketList;
	}

	@Override
	public void run() {

		if (mPacketList == null) {
			return;
		}
		
		synchronized (mutex) {
			while (!this.isSending) {
				try {
					mutex.wait();
				} catch (InterruptedException e) {
					throw new IllegalStateException("Wait() interrupted!", e);
				}
			}
		}
		SWPacket packet = null;
		if (os == null){
			return ;
		}
		int s = 10000;
		byte b[] = new byte[s];
		b[0] = 1;
		try {
			while (this.isSending) {
				packet = mPacketList.getPacket();
				if (packet == null) {
					Thread.sleep(200);
					continue;
				}
				Thread.sleep(100);
				
				int size = packet.getSize();
				Log.d(TAG, "size "+size+"  lenght  "+packet.getBuf().length);
				if (s > 0) {
					os.flush();
					os.write(int2ByteArray(size));
					os.flush();
					os.write(packet.getBuf());
					os.flush();
				} 
				if (packet.isFlag()){
					setSending(false);
				}
			}
			os.flush();
			byte[] end = { 0, 0, 0, 0 };
			os.write(end);
			
		} catch (Exception e) {
			throw new IllegalStateException("senddata() fail!", e);
		}
	
	}

	public void setSending(boolean isSending) {
		synchronized (mutex) {
			this.isSending = isSending;
			if (this.isSending) {
				mutex.notify();
			}
		}
	}

	public boolean isSending() {
		synchronized (mutex) {
			return isSending;
		}
	}

	public boolean openTCP(){
		try {
			socket = new Socket(ADD, PORT);
			socket.setTcpNoDelay(true);
			Log.d(TAG, ""+socket.getSendBufferSize());
			socket.setSendBufferSize(50000);
			Log.d(TAG, ""+socket.getSendBufferSize());
		} catch (IOException e1) {
			//throw new IllegalStateException("socket() init fail!", e1);
			return false;
		}
		if (socket == null){
			return false;
		}
		try {			
			os = socket.getOutputStream();			
		} catch (IOException e1) {
			//throw new IllegalStateException("OutputStream() init fail!", e1);
			return false;
		}
		
		if (os != null){
			return true;
		}
		return false;
	}
	
	public boolean closeTCP() {

		if (os != null) {
			try {
				os.flush();
				os.close();
			} catch (Exception e) {
				return false;
			}

			os = null;
		}
		if (socket != null) {
			try {
				socket.close();
			} catch (IOException e) {
				return false;
			}
			socket = null;
		}
		return true;
	}
	
	private static byte[] int2ByteArray(int i){
		if (i <= 0)
			return null;
		byte[] result = new byte[4];
		result[3] = (byte)((i>>24) & 0xFF);
		result[2] = (byte)((i>>16) & 0xFF);
		result[1] = (byte)((i>>8) & 0xFF);
		result[0] = (byte)(i & 0xFF);
		for(int a=0;i<4;a++)
		{
			Log.e(ADD, ""+result[a]);
		}
		return result;
	}

}
