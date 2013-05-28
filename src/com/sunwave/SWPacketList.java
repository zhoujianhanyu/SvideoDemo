package com.sunwave;

import java.util.ArrayList;
import java.util.List;

import android.util.Log;

public class SWPacketList {

	private static final String TAG = "SWPacketList";

	private List<SWPacket> mPacketList;
	private int index;
	private SWPacket mPacket;

	public SWPacketList() {

		mPacketList = new ArrayList<SWPacket>();
		index = 0;
	}

	public synchronized boolean setPacket(byte[] buf, int len, int isover) {
		
		Log.i(TAG, ""+len);
		if (mPacketList != null) {
			mPacket = new SWPacket();
			if (len > 0) {
				mPacket.setSize(len);
				mPacket.setBuf(buf);
			}  
			if (isover < 0) {
				mPacket.setFlag(true);
			}
			mPacketList.add(mPacket);
			index++;
			return true;
		}

		return false;
	}

	public synchronized SWPacket getPacket() {

		SWPacket packet = null;
		if (mPacketList != null && mPacketList.size() > 0) {
			packet = mPacketList.remove(0);
			index--;
		}
		return packet;

	}

	public synchronized boolean clearPacket() {

		if (mPacketList != null) {
			mPacketList.clear();
			mPacketList = null;
		}
		return false;

	}

	public synchronized int getIndex() {

		return index;
	}

	public class SWPacket {

		private int size = 0;
		private byte[] buf;
		private boolean flag = false;
		
		public int getSize() {
			return this.size;
		}

		public void setSize(int size) {
			this.size = size;
		}

		public byte[] getBuf() {
			return this.buf;
		}

		public void setBuf(byte[] buf) {
			this.buf = buf;
		}

		public boolean isFlag() {
			return this.flag;
		}

		public void setFlag(boolean flag) {
			this.flag = flag;
		}

	}
}
