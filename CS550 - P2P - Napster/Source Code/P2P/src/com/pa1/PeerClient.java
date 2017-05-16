package com.pa1;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Properties;


public class PeerClient {
	public String serverHostname;
	public String fileDir;
	public String fileDownloadDir;
	public int serverPort;
	public int peerPort;
	public boolean isAutoIndexingEnabled;

	/**
	 * constructor
	 * @throwsIOException
	 */
	public PeerClient() throws IOException {
		Properties prop = new Properties();
		prop.load(new FileInputStream("config.properties"));
		
		setDefaults(prop, Integer.parseInt(prop.getProperty("peer.port")));
	}
	
	/**
	 * constructor
	 * @param peerPort
	 */
	public PeerClient(int peerPort) throws IOException  {
		Properties prop = new Properties();
		prop.load(new FileInputStream("config.properties"));
		
		setDefaults(prop, peerPort);

	}
	
	private void setDefaults(Properties prop, int peerPort) throws IOException  {
		this.serverHostname = prop.getProperty("server.ipAddress");
		this.isAutoIndexingEnabled = Boolean.valueOf(prop.getProperty("peer.autoIndexingEnabled"));
		
		this.serverPort = Integer.parseInt(prop.getProperty("server.port"));
		this.peerPort = peerPort;
		
		String peerDir = prop.getProperty("peer.fileDir") + "/" + peerPort;
		this.fileDir = peerDir + "/" + "share/";
		this.fileDownloadDir = peerDir + "/" + "downlaods/";
		
		if (!Files.exists(Paths.get(peerDir))) {
			new File(this.fileDir).mkdirs();
			new File(this.fileDownloadDir).mkdirs();
		}
		
		Thread watcherThread = new Thread(new DirectoryWatcher(this.fileDir, this, this.isAutoIndexingEnabled));
		watcherThread.start();
	}
	
	
	/**
	 * Sends request to the server to register the file
	 * @param peerInfo
	 */
	public void registerFile(HashMap<String, String> peerInfo){
		try {
			if (!Files.exists(Paths.get(fileDir + peerInfo.get("fileName")))) {
				System.out.println("File does not exist on peer");
				return;
			}
			Socket socketServer = new Socket(serverHostname, serverPort);
			
			ObjectOutputStream objectOutputStream = new ObjectOutputStream(socketServer.getOutputStream());
			objectOutputStream.writeObject(peerInfo);
			objectOutputStream.flush();
			
			ObjectInputStream  objectInputStream = new ObjectInputStream(socketServer.getInputStream());
			Object obj = objectInputStream.readObject();
			if (obj instanceof Boolean) {
				if ((Boolean) obj)
					System.out.println("Peer - " +  peerInfo.get("peerID") + "|" + peerInfo.get("portNo") +": "+ peerInfo.get("fileName")+" registered");
			}
			
			socketServer.close();
			
		} catch (UnknownHostException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	
	/**
	 * Sends request to the server to search the file
	 * @param fileName
	 */
	@SuppressWarnings({ "unchecked", "resource" })
	public ArrayList<String> searchFile(String fileName) {		
		try {
			Socket serverSocket = new Socket(serverHostname, serverPort);
			ObjectOutputStream objectOutputStream = new ObjectOutputStream(serverSocket.getOutputStream());
			objectOutputStream.writeObject(fileName);
			objectOutputStream.flush();
			
			ObjectInputStream  objectInputStream = new ObjectInputStream(serverSocket.getInputStream());
			Object obj = objectInputStream.readObject();
			
			if (obj instanceof ArrayList) {
				ArrayList<String> peerList =  (ArrayList<String>)obj;
				int count = 0;
				System.out.println("\nFile - "+ fileName+" found at these peers :");
				for (String peerId : peerList)
					System.out.println("Peer "+ count++ +"  -  "+peerId);
				return peerList;
			}
			else {
				System.out.println("\nFile Not found on server..");
			}
			
			serverSocket.close();
		} catch (IOException | ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
	}
	
	/**
	 * Sends request to the server to download the file from other peer
	 * @param peerId
	 * @param fileName
	 * @param downloadDir
	 */
	public void downlodFile(String peerId, String fileName, String downloadDir) {
		//Receive file
		FileReceiver fileReceiver = new FileReceiver();
		fileReceiver.receiveFile(peerId, fileName, downloadDir);
	}
	
	
	/**
	 * Sends request to the server to delete the index of the file
	 * @param peerInfo
	 */
	public void deleteFile(HashMap<String, String> peerInfo) {
		try {
			Socket socketServer = new Socket(serverHostname, serverPort);
			
			ObjectOutputStream objectOutputStream = new ObjectOutputStream(socketServer.getOutputStream());
			objectOutputStream.writeObject(peerInfo);
			objectOutputStream.flush();
			
			socketServer.close();
			
		} catch (UnknownHostException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
