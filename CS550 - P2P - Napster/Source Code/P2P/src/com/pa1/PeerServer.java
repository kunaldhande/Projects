package com.pa1;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class PeerServer implements Runnable{

	private int port;
	private String fileDir;
	
	public PeerServer(int port, String fileDir) {
		this.port = port;
		this.fileDir = fileDir;
	}
	
	/**
	 * Listen for each file download request and sends file to the requesting peer
	 */
	@SuppressWarnings("resource")
	@Override
	public void run() {
		try {
			ServerSocket serverSocket = new ServerSocket(port);
			while (true){
				Socket client = serverSocket.accept();
				Thread clientThread = new Thread(new FileSender(client, fileDir));
				clientThread.start();
			}
		} catch (IOException e) {
			System.out.println("Could not start the file server..Please check if the port is available");
			e.printStackTrace();
		}
	}
}
