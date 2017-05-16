package com.iit.pa2;

import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;

/**
 * Class to handle the connection with neighboring peers
 */
public class NeighborHandler extends Thread{
	
	private int TTL;
	private String fileName;
	private String IP;
	private int portNo;
	private int fromPeerID;
	private int toPeerID;
	private String msgID;
	private HitQueryResult hitQueryResult = new HitQueryResult();
	
	/**
	 * 
	 * parameterized constructor
	 * Initializes NeighborHandler
	 * @param fileName
	 * @param IP
	 * @param portNo
	 * @param fromPeerID
	 * @param toPeerID
	 * @param msgID
	 * 
	 */
	NeighborHandler(String fileName, String IP, int portNo, int fromPeerID, int toPeerID, String msgID, int TTL){
		this.fileName = fileName;
		this.IP = IP;
		this.portNo = portNo;
		this.fromPeerID = fromPeerID;
		this.toPeerID = toPeerID;
		this.msgID = msgID;
		this.TTL = TTL;
	}
	
	/**
	 * Getter method
	 * This method returns the hit query result
	 */
	public HitQueryResult getValue(){
		return hitQueryResult;
	}
	
	
	/**
	 * Thread code
	 * Establishes connection with Requesting server
	 * 
	 */
	@Override
	public void run() {
		PeerInterface peer=null;
		try {
			// Establish the connection with RMI
			peer = (PeerInterface) Naming.lookup("rmi://" + IP + ":" + portNo + "/peerServer");
			
			// Call RMI method query
			hitQueryResult = peer.query(fileName, fromPeerID, msgID, TTL);	
			
		} catch (MalformedURLException | RemoteException | NotBoundException e) {
			 System.out.println("Unable to connect to " + toPeerID +"\nReason: "+ e.getMessage());
		}
		
	}
}
