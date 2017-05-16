package com.pa1;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Properties;

public class IndexServer {
	private ServerSocket serverSocket;
	private int port;
	
	public IndexServer() throws FileNotFoundException, IOException {
		Properties prop = new Properties();
		prop.load(new FileInputStream("config.properties"));
		this.port = Integer.parseInt(prop.getProperty("server.port"));
	}
	
	/**
	 * Keeps on listening for the file register and search request
	 * Handles multiple clients by creating independent thread for each client
	 * @throws IOException
	 */
	public void startServer () throws IOException{
		System.out.println("Server starting at Port : " + port);

		// Start server socket to accept request from peer.
		serverSocket = new ServerSocket(port);
		System.out.println("Server Started\n");
		
		while (true){
			Socket client = serverSocket.accept();
			// With threading approach, this can accept multiple requests from peer
			Thread clientThread = new Thread(new HandlePeers(client));
			clientThread.start();
		}
	}
	
	/**
	 * Main Function
	 * @param args
	 * @throws IOException
	 */
	public static void main(String[] args) throws IOException {
		// Initializing Indexing server
		IndexServer indexServer = new IndexServer();
		indexServer.startServer();
	}
}
