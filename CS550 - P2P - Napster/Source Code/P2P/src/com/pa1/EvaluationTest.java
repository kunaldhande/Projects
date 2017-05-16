/**
 * 
 */
package com.pa1;

import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.concurrent.ThreadLocalRandom;

public class EvaluationTest {
	
	static int noOfReq= 1000;
	static int noOfPeers = 3;
	static ArrayList<PeerClient> client = new ArrayList<PeerClient>();
	ArrayList<String> registeredFiles = new ArrayList<String>();
	
	public static void main(String[] args) throws IOException {
		EvaluationTest test = new EvaluationTest();
		for (int i = 0; i< noOfPeers; i++)
			client.add(new PeerClient(9990+i));
		
		test.testRegisterFiles();
		test.testSearchFile();
		
	}

	private void testRegisterFiles() {
		long startTime = System.currentTimeMillis();
		
		System.out.println("\nSending "+ noOfReq +" register requests");
		System.out.println("Start time : " + getReadableDate(startTime));
		
		for (int i = 0; i < noOfReq; i++) {
			HashMap<String, String> peerInfo = new HashMap<String, String>();
			int index = ThreadLocalRandom.current().nextInt(0, noOfPeers);
			peerInfo.put("peerID", P2PFileTransfer.getIPAddress());
			peerInfo.put("portNo", String.valueOf(client.get(index).peerPort));
			peerInfo.put("operation", "register");
			peerInfo.put("fileName", "file" + ThreadLocalRandom.current().nextInt(0, noOfReq));
			registeredFiles.add(peerInfo.get("fileName") + "-" + peerInfo.get("portNo"));
			client.get(index).registerFile(peerInfo);
		}
		long endTime = System.currentTimeMillis();
		System.out.println("End time : " + getReadableDate(endTime));
		System.out.println("Execution Time : " + (endTime - startTime) + "ms");
		
	}

	private void testSearchFile() {
		long startTime = System.currentTimeMillis();
		System.out.println("\nSending "+ noOfReq +" search requests");
		System.out.println("Start Time : " + getReadableDate(startTime));
		
		for (String peer : registeredFiles) {
			String file = peer.substring(0, peer.indexOf('-'));
			int peerIndex = Integer.valueOf(peer.substring(peer.length()-1));
			client.get(peerIndex).searchFile(file);
		}

		long endTime = System.currentTimeMillis();
		System.out.println("End Time : " + getReadableDate(endTime));
		System.out.println("Execution Time : " + (endTime - startTime) + "ms");
		
	}
	
	private String getReadableDate(long ms) {
		SimpleDateFormat sdf = new SimpleDateFormat("MMM dd,yyyy HH:mm");    
		Date resultdate = new Date(ms);
		return sdf.format(resultdate);
	}
}

