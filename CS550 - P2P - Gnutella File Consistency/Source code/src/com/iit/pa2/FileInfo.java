package com.iit.pa2;

import java.io.Serializable;

public class FileInfo implements Serializable{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	public String fileName;
	public int versionNum;
	public PeerDetails originalServer;
	public PeerDetails server;
	public String conState;
	public int TTR;
	public long lastModifiedTime;

	
	public FileInfo(String s, PeerDetails peer, int ttr) {
		fileName = s;
		versionNum = 0;
		originalServer = peer;
		conState = "Valid";
		TTR = ttr;
		lastModifiedTime = System.currentTimeMillis();
	}
	
	public FileInfo(FileInfo file) {
		this.fileName = file.fileName;
		this.originalServer = file.originalServer;
		this.versionNum = file.versionNum;
		this.conState = file.conState;
		this.TTR = file.TTR;
		this.lastModifiedTime = file.lastModifiedTime;
	}
	
	public FileInfo() {
		// TODO Auto-generated constructor stub
	}

	public void versionIncrease() {
		versionNum += 1;
	}
}
