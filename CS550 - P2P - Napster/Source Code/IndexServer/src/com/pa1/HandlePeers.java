package com.pa1;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;



public class HandlePeers implements Runnable {
	
	Socket client;
	Map<String, ArrayList<String>> map;
	static Map<String, ArrayList<String>> fileIndex;
	
	/**
	 * Parameterized constructor 
	 * to handle request from Peers
	 * @param client
	 */
	public HandlePeers(Socket client) {
		if (null == fileIndex)
			fileIndex = new ConcurrentHashMap<String, ArrayList<String>>();
		this.client = client;
	}
	

	/**
	 * Reads request from client and identifies if the request is file register request 
	 * or search file request or delete request
	 */
	@Override
	public void run() {
		try {
			//Read client request and distinguish
			ObjectInputStream inFromClient = new ObjectInputStream(client.getInputStream());
			Object obj = inFromClient.readObject();
			
			if(obj instanceof HashMap){
				@SuppressWarnings("unchecked")
				HashMap<String, String> params = (HashMap<String, String>) obj;
				if (params.get("operation").equals("register"))
					registry(params.get("peerID"), params.get("portNo"), params.get("fileName"));
				else
					delete(params.get("peerID"), params.get("portNo"), params.get("fileName"));
			}
			
			else if(obj instanceof String){
				String fileName = (String) obj;
				lookup(fileName);
			}
			
			else{
				System.out.println("Invalid request from client..\n");
			}
				
		} catch (ClassNotFoundException e) {
			System.out.println("Data type from client not supported");
		} catch (IOException e) {
			System.out.println("Error reading data from client");
		} finally{
			try {
				client.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
	}

	/**
	 * Registers file on the indexing server
	 * @param peerID
	 * @param portNo
	 * @param fileName
	 */
	private void registry(String peerID, String portNo, String fileName) {
		System.out.println("\nFile register request from :" + peerID + " Port : " + portNo);
		ArrayList<String> peerList;
		
		try {
			ObjectOutputStream objOutStream = new ObjectOutputStream(client.getOutputStream());
		
			// Check if file already  registered on the server if yes then it add new peer to the list 
			// otherwise create new index
			Boolean flag = true;
			if (fileIndex.containsKey(fileName)) {
				peerList = fileIndex.get(fileName);
				
				for (Iterator<String> it = peerList.iterator(); it.hasNext(); ) {
				    String peer = it.next();
				    if (!peer.equalsIgnoreCase(peerID+"|"+portNo)) {
						flag = true;
					}
					else {
						flag = false;
						break;
					}
				}
					
				if (flag)
					peerList.add(peerID+"|"+portNo);
				
			}
			else {
				peerList = new ArrayList<String>();
				peerList.add(peerID+"|"+portNo);
				fileIndex.put(fileName, peerList);
			}
			objOutStream.writeObject(flag);
			objOutStream.flush();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	/**
	 * Searches given filename on indexing server
	 * @param fileName
	 */
	private void lookup(String fileName) {
		if (fileIndex == null) {
			System.out.println("There are no files registered on the server");
		}else {		
			try {
				ArrayList<String> peerList = fileIndex.get(fileName);
				ObjectOutputStream objOutStream = new ObjectOutputStream(client.getOutputStream());
				
				if (peerList != null){
					System.out.println("File found!!");
					objOutStream.writeObject(peerList);
					objOutStream.flush();
					
				}else{
					objOutStream.writeObject("NotFound");
					objOutStream.flush();
					System.out.println("File Not Found!!\n");	
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
	
	/**
	 * Deletes file on the indexing server
	 * @param peerID
	 * @param portNo
	 * @param fileName
	 */
	private void delete(String peerID, String portNo, String fileName) {
		ArrayList<String> peerList;
		if (fileIndex.containsKey(fileName)) {
			peerList = fileIndex.get(fileName);
			
			// Removing file index from specific port on deletion of file.
			for(String peer : peerList) {
				if (peer.equals(peerID+"|"+portNo)){
					peerList.remove(peer);
					break;
				}
			}
			if (peerList.size()==0)
				fileIndex.remove(fileName);
		}
	}
}



