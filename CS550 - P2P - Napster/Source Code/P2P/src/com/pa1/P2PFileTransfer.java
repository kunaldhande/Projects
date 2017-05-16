/**
 * 
 */
package com.pa1;

import java.io.IOException;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.InputMismatchException;
import java.util.Scanner;


public class P2PFileTransfer {	
	
	public static void main(String[] args) throws UnknownHostException, IOException {
		PeerClient client = new PeerClient();
		//Start the file sharing first
		PeerServer server = new PeerServer(client.peerPort, client.fileDir);
		Thread fileSender = new Thread(server);
		fileSender.start();
				
		Scanner scan = new Scanner(System.in);
		
		//Options to the user
		while(true){
			System.out.println("\n\n");
			System.out.println("*************************************************************");
			System.out.println("*  A Simple Napster Style Peer to Peer File Sharing System  *");
			System.out.println("*************************************************************");
			System.out.println("1. Register file on server");
			System.out.println("2. Search and download file");
			System.out.println("3. Search and delete file on server");
			System.out.println("4. Exit");
			System.out.println("Enter your choice : ");
			String userChoice = scan.next();
			
			switch (userChoice){
				case "1":
					HashMap<String, String> peerInfo = new HashMap<String, String>();
					peerInfo.put("peerID", getIPAddress());
					peerInfo.put("portNo", String.valueOf(client.peerPort));
					peerInfo.put("operation", "register");
					System.out.println("\nEnter file Names :");
					peerInfo.put("fileName", scan.next());
					
					client.registerFile(peerInfo);
					break;
					
				case "2": 
					System.out.println("\nEnter name of the file to search :");
					String fileName = scan.next();
					ArrayList<String> peerList = client.searchFile(fileName);
					
					if (peerList != null) {
						System.out.println("\nDo you wish to download the file? (Y/N)");
						if("Y".equalsIgnoreCase(scan.next())){	
							int index = 1;
							while(true) {
								if (peerList.size()>1) {
									System.out.println("\nSelect the peer from which you want to download: ");
									int count = 0;
									for (String peerId : peerList)
										System.out.println(++count + "> "+peerId);
									
									try {
										index = scan.nextInt();
										if (peerList.size()+1 > index)
											break;
									}
									catch(InputMismatchException exception) {
									  System.out.println("Invalid Input");
									}
								}
							}
							System.out.println("\nConnecting to the peer (" + peerList.get(index-1) + ") to download the file");
							client.downlodFile(peerList.get(index-1), fileName, client.fileDownloadDir);
						}
					}
					
					
					break;
				
				case "3":
					System.out.println("\nEnter name of the file to search :");
					fileName = scan.next();
					peerList = client.searchFile(fileName);
					
					if (peerList != null) {
						int index = 1;
						while(true) {
							System.out.println("\nSelect the peer from which you want to delete file: ");
							int count = 0;
							for (String peerId : peerList)
								System.out.println(++count + "> "+peerId);
							
							try {
								index = scan.nextInt();
								if (peerList.size()+1 > index)
									break;
							}
							catch(InputMismatchException exception) {
							  System.out.println("Invalid Input");
							}
						}
						System.out.println("\nConnecting to the peer (" + peerList.get(index-1) + ") to delete the file");
						peerInfo = new HashMap<String, String>();
						int ind = peerList.get(index-1).indexOf("|");
						
						peerInfo.put("peerID", peerList.get(index-1).substring(0, ind));
						peerInfo.put("portNo", peerList.get(index-1).substring(ind+1));
						peerInfo.put("operation", "delete");
						peerInfo.put("fileName", fileName);
						
						client.deleteFile(peerInfo);
					}
					
					break;
					
				case "4":
					System.out.println("With this is action the files registered by you will not be available to other users to download.");
					System.out.println("Do you still want to exit? (Y/N)");
					if ("Y".equalsIgnoreCase(scan.next())){
						scan.close();
						System.exit(0);
					}
					break;
					
				default : 
					System.out.println("Invalid choice...try again");
			}
		}
	}

	
	
	/**
	 * Returns IP address of the system
	 * @return
	 * @throws SocketException
	 */
	public static String getIPAddress() {
		Enumeration<NetworkInterface> en;
		try {
			en = NetworkInterface.getNetworkInterfaces();
			while (en.hasMoreElements()){
				NetworkInterface nt = (NetworkInterface) en.nextElement();
				for (Enumeration<InetAddress> en2 = nt.getInetAddresses(); en2.hasMoreElements();){
					InetAddress addr = (InetAddress) en2.nextElement();
					if (!addr.isLoopbackAddress()){
						if (addr instanceof Inet4Address){
							return addr.getHostAddress();
						}
					}
				}
			}
		} catch (SocketException e) {
			e.printStackTrace();
		}
		return null;
	}
}

