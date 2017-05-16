package com.iit.pa2;

import java.io.IOException;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;



public class Test {
	static int noOfPeers = 10;
	static int noOfOperations = 200;
	static int noOfClients = 1;
	
	public static void main(String[] args) {
		ArrayList<Peer> peerInstances = new ArrayList<Peer>();
		for(int i=0; i< noOfPeers; i++)
			peerInstances.add(new Peer(i+1, 8000+i+1));
		
		testSearh(peerInstances);
	}
	
	static void testSearh(ArrayList<Peer> peerInstances) {	
		try {
			long start=System.currentTimeMillis();
			for(int i=0;i<noOfOperations;i++) {
				int index = ThreadLocalRandom.current().nextInt(0, noOfPeers);
				Peer peerInstance = peerInstances.get(index);
				
				String searchFileName = "file" + ((i%10)+1);
				System.out.println("Searching file: "+ searchFileName);
				ArrayList<PeerDetails> resultsFromPeer = peerInstance.search(searchFileName);
				for (PeerDetails resultFromPeer: resultsFromPeer)
					peerInstance.downloadFile(resultsFromPeer, resultFromPeer.peerID, searchFileName);
				
			}
			long end=System.currentTimeMillis();
			System.out.println("Total time ("+ noOfClients +"): "+ (double)(end-start)/noOfOperations);

			
		} catch (InterruptedException | IOException e) {
			e.printStackTrace();
		}
	}
}
