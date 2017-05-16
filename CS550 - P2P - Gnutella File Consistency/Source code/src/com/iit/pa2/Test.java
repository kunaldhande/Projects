package com.iit.pa2;

import java.io.IOException;
import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;



public class Test {
	static int noOfPeers = 10;
	static int noOfOperations = 100;
	static int noOfClients = 1;
	
	public static void main(String[] args) {
		ArrayList<Peer> peerInstances = new ArrayList<Peer>();
		for(int i=0; i< noOfClients; i++)
			peerInstances.add(new Peer(i+1, 8000+i+1));
		
		testSearh(peerInstances);
	}
	
	static void testSearh(ArrayList<Peer> peerInstances) {	
		int invalidCount = 0;
		int totalCount = 0;
		try {
			long start=System.currentTimeMillis();
			for(int i=0;i<noOfOperations;i++) {
				int index = ThreadLocalRandom.current().nextInt(0, noOfClients);
				Peer peerInstance = peerInstances.get(index);
				
				String searchFileName = "file" + ((i%10)+1);
				System.out.println("Searching file: "+ searchFileName);
				ArrayList<PeerDetails> resultsFromPeer = peerInstance.search(searchFileName);
				if (resultsFromPeer.size() > 0) {
					index = ThreadLocalRandom.current().nextInt(0, resultsFromPeer.size());
					FileInfo downloadedinfo = peerInstance.downloadFile(resultsFromPeer, resultsFromPeer.get(index).peerID, searchFileName);
					
					PeerInterface peerServer = (PeerInterface) Naming.lookup("rmi://localhost:" + downloadedinfo.originalServer.portNo + "/peerServer");
					FileInfo modifiedFileInfo = peerServer.obtainInfo(searchFileName);
					
					if (modifiedFileInfo.lastModifiedTime > downloadedinfo.lastModifiedTime)
						invalidCount++;
					totalCount++;
				}
			}
			
			long end=System.currentTimeMillis();
			System.out.println("Total time ("+ noOfClients +"): "+ (double)(end-start)/noOfOperations);
			System.out.println("===================================================================");
			System.out.println(invalidCount);
			System.out.println("===================================================================");

			
		} catch (InterruptedException | MalformedURLException | RemoteException | NotBoundException e) {
			e.printStackTrace();
		}
	}
}
