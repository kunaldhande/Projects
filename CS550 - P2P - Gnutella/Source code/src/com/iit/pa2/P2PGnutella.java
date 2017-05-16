package com.iit.pa2;

import java.util.ArrayList;
import java.util.Scanner;

/**
 * Main class
 */
public class P2PGnutella {
	
	@SuppressWarnings("resource")
	public static void main(String args[]) {
		if (args.length!=2) {
			System.out.println("Please pass arguments as arg1: PeerID and arg2=Port number");
			System.exit(0);
		}
		
		Peer peerInstance = new Peer(Integer.parseInt(args[0]), Integer.parseInt(args[1]));

		try {
			Scanner scan = new Scanner(System.in);
			

			// Options to select from menu
			while (true) {
				System.out.println("");
				System.out.println("");
				System.out.println("******************************************************************");
				System.out.println("**** A Simple Gnutella Style Peer to Peer File Sharing System ****");
				System.out.println("******************************************************************");
				System.out.println("1. Search and Download File");
				System.out.println("2. Exit");
				System.out.println("Select your choice: ");
				
				// Switch
				switch (scan.next()) {
					case "1":
						System.out.println("Enter file name to search:");
						String searchFileName = scan.next();
						ArrayList<PeerDetails> resultsFromPeer = peerInstance.search(searchFileName);
						
						System.out.println("\n\n'" + searchFileName + (resultsFromPeer.size() == 0 ? "' not found in the nework": " found in the nework at below peers"));
						
						// Display the peers list where the searchFilename file existed.
						for (PeerDetails resultFromPeer: resultsFromPeer)
							System.out.println(resultsFromPeer.indexOf(resultFromPeer)+1 + ". Peer" + resultFromPeer.peerID + " running on 127.0.0.1:" + resultFromPeer.portNo);

						
						// call method for download functionality
						if (resultsFromPeer.size()>0)
							peerInstance.selectPeerToDownloadFile(resultsFromPeer, searchFileName);
						break;
						
					case "2":
						System.exit(1);
						
					default:
						System.out.println("Invalid selection");
						break;
				}
			}
		}
		catch (Exception e) {
			System.out.println(e.getMessage());
		}
	}
}
