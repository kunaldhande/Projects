package com.iit.pa2;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.rmi.*;
import java.rmi.server.*;
import java.util.ArrayList;
import java.util.List;

/**
 * Class to implement interface PeerInterface
 * @serial serialVersionUID
 */
public class PeerInterfaceRemote extends UnicastRemoteObject implements PeerInterface {
	private static final long serialVersionUID = 1L;
	
	private String sharedDirectory;
	private int peerID;
	private int currentPeerPortNo;
	private ArrayList<String> localFiles=new ArrayList<String>();
	private Peer currentPeer;
	private ArrayList<String> processdMsgIds;
	

	/**
	 * 
	 * parameterized constructor
	 * Initializes NeighborHandler
	 * @param sharedDir
	 * @param peerID
	 * @param currentPeerPortNo
	 * @param localFiles
	 * @param currentPeer
	 * 
	 */
	PeerInterfaceRemote(String sharedDir, int peerID, int currentPeerPortNo, ArrayList<String> localFiles, Peer currentPeer) throws RemoteException {
		super();
		// store the constructor parameters
		this.sharedDirectory = sharedDir;
		this.peerID = peerID;
		this.currentPeerPortNo=currentPeerPortNo;
		this.localFiles=localFiles;
		this.currentPeer = currentPeer;
		this.processdMsgIds=new ArrayList<String>();
	}
	
	/**
	 * RMI method to download file
	 * @param filename
	 */
	public synchronized byte[] obtain(String filename) throws RemoteException {
		// Remote method for downloading the file
		byte[] fileBytes = null;
		String fullFileName = sharedDirectory + "/" + filename;
		try {
			fileBytes = new byte[(int) (new File(fullFileName)).length()];
			BufferedInputStream input = new BufferedInputStream(new FileInputStream(fullFileName));
			// get the data in bytes
			input.read(fileBytes, 0, fileBytes.length);
			input.close();
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
		return fileBytes;

	}

	/**
	 * RMI method for handling the search request
	 * @param fileName
	 * @param fromPeerID
	 * @param msgID
	 * @param TTL
	 */
	public HitQueryResult query(String fileName, int fromPeerID, String msgID, int TTL) throws RemoteException {
		
		ArrayList<PeerDetails> availableAt = new ArrayList<PeerDetails>();
		ArrayList<PeerDetails> neighboringPeers = new ArrayList<PeerDetails>();
		ArrayList<String> path = new ArrayList<String>();
		HitQueryResult hitqueryResult=new HitQueryResult();
		ArrayList<NeighborHandler> peerThreads = new ArrayList<NeighborHandler>();
		
		synchronized(this){
			// checking for the duplicate request
			if (processdMsgIds.contains(msgID)){
				System.out.println("Incoming Request to peer "+peerID+": From - "+fromPeerID+" Duplicate Request - Already searched in this peer- with message id - " + String.valueOf(msgID));
				return hitqueryResult;	
			}
			
			// Checking for TTL (Time To Live)
			if (TTL==0) {
				return hitqueryResult;	
			}
		} 
		
		processdMsgIds.add(msgID);
		System.out.println("Incoming Request to peer "+peerID+": From - "+fromPeerID+" Search locally and send request to neighbours for msg id- " + String.valueOf(msgID));
		
		
		// Search the filename among the local files of current peer
		if(searchInCurrentPeer(localFiles,fileName)){
			System.out.println("Local Search: File Found in the current peer");
			PeerDetails temp=new PeerDetails();
			temp.hostIP = "localhost";
			temp.peerID = peerID;
			temp.portNo = currentPeerPortNo;
			availableAt.add(temp);
		}
		else 
			System.out.println("Local Search: File not found in the current peer");

		
		// Read the configuration file to get the neighboring peers details.
		currentPeer.getNeighboringPeers(neighboringPeers, peerID);
		if (neighboringPeers.size() == 0)
			path.add(Integer.toString(peerID));
		
		TTL--;
		List<Thread> threads = new ArrayList<Thread>();
		for(PeerDetails neighborPeer : neighboringPeers){
			if (neighborPeer.peerID == fromPeerID)
				continue;
			
			System.out.println("Outgoing Request from peer " + peerID + ": Sending request to "+neighborPeer.peerID + " "+ neighborPeer.portNo);
			NeighborHandler ths = new NeighborHandler(fileName, neighborPeer.hostIP, neighborPeer.portNo,peerID, neighborPeer.peerID, msgID, TTL);  
			Thread ts = new Thread(ths);
			ts.start();

			// store the instances to get the return values after all the threads finish the exiecution
			threads.add(ts);
			peerThreads.add(ths);
		}
		
		
		try {
			for (Thread thread: threads)
				thread.join();
			for (Thread thread: peerThreads)
				thread.join();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	
		
		for (NeighborHandler peerThread: peerThreads){
			// Retrieve the result of the thread request
			HitQueryResult temp =  peerThread.getValue();
			if(temp.searchResults.size()>0)
				availableAt.addAll(temp.searchResults);
			
			for (int count=0;count<temp.paths.size();count++)
				path.add(peerID + temp.paths.get(count));
		}
		
		if (path.size()==0)
			path.add(Integer.toString(peerID));

		// send the result back to the sender
		System.out.println("HitQuery: Send following result back to "+fromPeerID);
		for (PeerDetails peerAt: availableAt)
			 System.out.println("--Found at Peer" + peerAt.peerID+" on localhost:" + peerAt.portNo);
		
		hitqueryResult.searchResults.addAll(availableAt);
		hitqueryResult.paths.addAll(path);

		return hitqueryResult;
	}

	/**
	 * Method to search file in current peer
	 * @param localFiles
	 * @param localFiles
	 */
	private Boolean searchInCurrentPeer(ArrayList<String> localFiles, String filename) {
		return !(localFiles.indexOf(filename) == -1);
	}
}
