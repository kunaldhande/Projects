package com.iit.pa2;

import java.io.IOException;
import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;



public class Test2 {
	static int noOfPeers = 10;
	static int noOfOperations = 2;
	static int noOfClients = 1;
	
	public static void main(String[] args) {
		ArrayList<Peer> peerInstances = new ArrayList<Peer>();
		for(int i=noOfClients; i< noOfPeers; i++)
			peerInstances.add(new Peer(i+1, 8000+i+1));
		
		modifyFiles(peerInstances);
	}
	
	static void modifyFiles(ArrayList<Peer> peerInstances) {
		
		long start=System.currentTimeMillis();
		for(int i=0;i<noOfOperations;i++) {
			int index = ThreadLocalRandom.current().nextInt(noOfClients, noOfPeers);
			Peer peerInstance = peerInstances.get(index);
			
			FileInfo info = peerInstance.localFiles.get(ThreadLocalRandom.current().nextInt(0, peerInstance.localFiles.size()));
			
			peerInstance.modifyFile(info.fileName);
		}
		
		long end=System.currentTimeMillis();
		System.out.println("Total time ("+ noOfClients +"): "+ (double)(end-start)/noOfOperations);
	}
}
