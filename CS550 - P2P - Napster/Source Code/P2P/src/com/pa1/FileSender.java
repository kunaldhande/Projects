/**
 * 
 */
package com.pa1;

import java.io.BufferedInputStream; 
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.net.Socket;


public class FileSender implements Runnable {
	private Socket client;
	private String fileDir;
	
	/**
	 * parameterized constructor
	 * @param client
	 * @param fileDir
	 */
	public FileSender(Socket client, String fileDir) {
		this.client = client;
		this.fileDir = fileDir;
	}
	
	/**
	 * Sends file to the requesting peer
	 */
	@Override
	public void run() {
		try{	
			ObjectInputStream objectInputStream = new ObjectInputStream(client.getInputStream());
			DataOutputStream dataOutputStream = new DataOutputStream(client.getOutputStream());
			
			// Receive filename from requesting peer
			String fileName = (String) objectInputStream.readObject();
			
			// Start sending file to the requesting peer
			File file = new File (fileDir + fileName);

			byte[] byteArray = new byte[(int) file.length()];
			DataInputStream dataInputStream = new DataInputStream(new BufferedInputStream(new FileInputStream(file)));
			dataInputStream.readFully(byteArray, 0, byteArray.length);
			
			dataOutputStream.writeUTF(fileName);
			dataOutputStream.writeLong(byteArray.length);
			dataOutputStream.write(byteArray, 0 ,byteArray.length);
			
			dataOutputStream.flush();
			dataInputStream.close();
			
		}catch (ClassNotFoundException e) {
			System.out.println("Incorrect data type of the filename from requesting server.");
		} catch (IOException e) {
			System.out.println(e.getMessage());
		} 
	}

	
}
