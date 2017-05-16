package com.iit.pa2;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.Scanner;

/**
 * Class to search, download, index files, store neighbors
 */
public class Peer {
	
	private int TTL = 3;
	private int TTR;
	
	private String sharedDir;
	private int msgIDConuter = 0;
	private PeerInterface stub;
	private ArrayList<PeerDetails> resultsFromPeer;
	
	public ArrayList<FileInfo> localFiles = new ArrayList<FileInfo>();
	public ArrayList<FileInfo> downloadFiles = new ArrayList<FileInfo>();
	public PeerDetails peerDetails= new PeerDetails();
	public Boolean isPush;
	
	/**
	 * 
	 * parameterized constructor
	 * Initializes Peer
	 * @param peerID
	 * @param peerID
	 * 
	 */
	Peer(int peerID, int portNo) {
		this.peerDetails.peerID = peerID;
		this.peerDetails.portNo = portNo;
		this.peerDetails.hostIP = "127.0.0.1";
		this.sharedDir = "data/" + peerID;
		
		// Run peer as a server on a specific port
		setPeerApproach();
		runPeerAsServer(this.peerDetails.peerID, this.peerDetails.portNo, this.sharedDir);
		if (!isPush)
			autoupdate();
	}
	
	/*------------------ Start Change ------------------*/
	/**
	 * Method to set approach - Pus or Pull
	 */
	private void setPeerApproach() {
		InputStream input;
		try {
			Properties prop = new Properties();
			input = new FileInputStream("config.properties");
			prop.load(input);
			isPush = prop.getProperty("approach").equalsIgnoreCase("push");
			
			TTR = Integer.parseInt(prop.getProperty("TTR"));
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * Method to generate unique messageID
	 */
	public String getMsgID(){
		++msgIDConuter;
		return System.currentTimeMillis()+ "-" + peerDetails.peerID + "-"+ msgIDConuter;
	}
	/*------------------ End Change ------------------*/
	
	
	/**
	 * Method to send search request to neighboring peer
	 * @param searchFileName
	 * @exception InterruptedException
	 */
	public ArrayList<PeerDetails> search(String searchFileName) throws InterruptedException {
		List<Thread> threads = new ArrayList<Thread>();
		resultsFromPeer = new ArrayList<PeerDetails>();
		
		ArrayList<PeerDetails> neighboringPeers = new ArrayList<PeerDetails>();
		ArrayList<NeighborHandler> neighboringThreads = new ArrayList<NeighborHandler>();

		// Get Neighboring peers
		getNeighboringPeers(neighboringPeers, peerDetails.peerID);

		// Generate unique message id
		
		String msgId = getMsgID();
		
		// Loop through all the neighbor peers
		for (PeerDetails neighboringPeer: neighboringPeers) {
			
			NeighborHandler connection = new NeighborHandler(searchFileName, 
					neighboringPeer.hostIP, neighboringPeer.portNo, peerDetails.peerID,
					neighboringPeer.peerID, msgId,TTL, "download");
			Thread threadInstance = new Thread(connection);
			threadInstance.start();
			
			// Add connection thread instances
			threads.add(threadInstance);
			neighboringThreads.add(connection);
		}
		
		// Wait until all child threads finish execution
		for (Thread thread: threads)
			thread.join();

		// Get hit query result from all the neighbor peers
		System.out.println("\n");
		for (NeighborHandler neighboringThread: neighboringThreads) {
			HitQueryResult hitQueryResult = (HitQueryResult) neighboringThread.getValue();
			if (hitQueryResult.searchResults.size() > 0){
				resultsFromPeer.addAll(hitQueryResult.searchResults);
			}
		}
		return resultsFromPeer;
	}
	
	
	/**
	 * Method to store and index all local files in shared directory
	 * @param sharedDir
	 * @param localFiles
	 */
	public void getLocalFilesFromSharedFolder(String sharedDir) {
		for (String file:  (new File(sharedDir)).list())
			localFiles.add(new FileInfo(file, peerDetails, TTR));
	}

	
	/**
	 * Method to run Peer as server with RMI
	 * @param peerId
	 * @param portNo
	 * @param sharedDir
	 */
	public void runPeerAsServer(int peerId, int portNo, String sharedDir) {
		try {
			LocateRegistry.createRegistry(portNo);
			getLocalFilesFromSharedFolder(sharedDir+"/share");
			stub = new PeerInterfaceRemote(sharedDir + "/share",sharedDir + "/downloads", peerId, portNo, this);
			Naming.rebind("rmi://localhost:" + portNo + "/peerServer", stub);
			System.out.println("Peer " + peerId + " acting as server on 127.0.0.1:" + portNo);
			
			(new Thread(new DirectoryWatcher(this, sharedDir + "/share"))).start();
			
		} catch (Exception e) {
			System.out.println(e);
		}
	}

	
	/**
	 * Method to list all neighboring peers
	 * @param neighborPeers
	 * @param peerId
	 */
	public void getNeighboringPeers(ArrayList<PeerDetails> neighborPeers, int peerId) {
		InputStream input = null;

		try {
			// Read configuration file
			Properties prop = new Properties();
			input = new FileInputStream("config.properties");
			prop.load(input);
			
			// Iterate through all neighbor list
			for (String strNeighbor: prop.getProperty(peerId + ".neighbors").split(",")) {
				PeerDetails tempPeer = new PeerDetails();
				tempPeer.peerID = Integer.parseInt(strNeighbor);
				tempPeer.portNo = Integer.parseInt(prop.getProperty(strNeighbor + ".port"));
				tempPeer.hostIP = prop.getProperty(strNeighbor + ".ip");
				neighborPeers.add(tempPeer);
			}
			System.out.println("");
			input.close();
		} catch (IOException e) {
			System.out.println(e.getMessage());
			e.printStackTrace();
		}
	}

	
	/**
	 * Method to ask permission of user to download file from x peer
	 * @param searchResult_Peers
	 * @param fileName
	 * @param path
	 */
	@SuppressWarnings("resource")
	public void selectPeerToDownloadFile(ArrayList<PeerDetails> searchResult_Peers, String fileName) {
		try {
			Scanner scan = new Scanner(System.in);
			System.out.println("\nDo you wish to download the file? (Y/N): ");
			switch (scan.next().toLowerCase()) {
				case "y":
					System.out.println("Enter from which peer you want to download the file: ");
					downloadFile(searchResult_Peers, searchResult_Peers.get(Integer.parseInt(scan.next())).peerID, fileName);
					break;
			}
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
	}

	/**
	 * Method to download file from user selected peer
	 * @param resultsFromPeer
	 * @param peerId
	 * @param fileName
	 * @param path
	 * @exception IOException
	 */
	public FileInfo downloadFile(ArrayList<PeerDetails> resultsFromPeer, int peerId, String fileName) {
		FileInfo info = null;
		for (PeerDetails resultFromPeer : resultsFromPeer) {
			
			// Check for user selected peerID
			if (peerId == resultFromPeer.peerID) {
				
				// Get an object for peer server to download the file.
				PeerInterface peerServer;
				try {
					peerServer = (PeerInterface) Naming.lookup("rmi://localhost:" + resultFromPeer.portNo + "/peerServer");
					info = peerServer.obtainInfo(fileName);
					
					downloadFileFromPort(resultFromPeer.portNo, fileName);
					info.server = resultFromPeer;
					downloadFiles.add(info);
					break;
					
				} catch (MalformedURLException | RemoteException | NotBoundException e) {
					e.printStackTrace();
				}
				
			}
		}
		return info;
	}
	
	private void downloadFileFromPort(int port, String fileName) {
		try {
			// Get an object for peer server to download the file.
			PeerInterface peerServer = (PeerInterface) Naming.lookup("rmi://localhost:" + port + "/peerServer");
			
			// Call RMI method - obtain to download the file
			byte[] out = peerServer.obtain(fileName);
			
			// Create new file for current peer with the downloaded bytes of data
			BufferedOutputStream output = new BufferedOutputStream(new FileOutputStream(sharedDir  + "/downlaods/" + fileName));
			output.write(out, 0, out.length);
			output.flush();
			output.close();
			
		} catch (NotBoundException | IOException e) {
			e.printStackTrace();
		}
	}
	
	/*------------------ Start Change ------------------*/
	/**
	 * Method to auto-update TTR expired files
	 */
	private void autoupdate() {
		new java.util.Timer().schedule( 
	        new java.util.TimerTask() {
	            @Override
	            public void run() {
	                update();
	            }
	        }, 
	        TTR 
		);
	}
	
	/**
	 * Method to update files
	 */
	public void update() {
		for (FileInfo f : downloadFiles) {
        		
    		PeerInterface peerServer;
			try {
				peerServer = (PeerInterface) Naming.lookup("rmi://localhost:" + f.originalServer.portNo + "/peerServer");
				FileInfo info = peerServer.obtainInfo(f.fileName);
				
				if (info.lastModifiedTime > f.lastModifiedTime) {
					downloadFileFromPort(f.originalServer.portNo, f.fileName);
				}
			
			} catch (MalformedURLException | RemoteException | NotBoundException e) {
				e.printStackTrace();
			}
        }
	}
	
	/**
	 * Method to modify file content
	 */
	public void modifyFile(String filename) {
		try {
		    Files.write(Paths.get(sharedDir + "/share/" + filename), " ".getBytes(), StandardOpenOption.APPEND);
		}catch (IOException e) {
		    System.out.println(e);
		}
	}
	
	/*------------------ End Change ------------------*/
}
