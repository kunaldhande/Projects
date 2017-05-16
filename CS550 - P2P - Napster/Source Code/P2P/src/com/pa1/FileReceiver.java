/**
 * 
 */
package com.pa1;

import java.io.DataInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;


public class FileReceiver {
	
	static int BUFF_SIZE = 1024;

	/**
	 * Receives file from the file server
	 * @param peerAddress
	 * @param fileName
	 * @param fileDir
	 */
	public void receiveFile(String peerAddress, String fileName, String fileDir){
		// Split the address string to get IP and port
		int index = peerAddress.indexOf('|');
		String peerIp = peerAddress.substring(0, index);
		int peerPort = Integer.parseInt(peerAddress.substring(index+1, peerAddress.length()));
		
		try {
			Socket peerSocket = new Socket(peerIp, peerPort);
			System.out.println("Connected with the peer => peerIP:" + peerIp + ", peerPort:" + peerPort);
			
			System.out.println("\nDownloading file....");
			
			// Sending fileName to the peer from which file is to be retrieve
			ObjectOutputStream objectOutputStream = new ObjectOutputStream(peerSocket.getOutputStream());
			objectOutputStream.writeObject(fileName);
			objectOutputStream.flush();
			
			// Receiving file from another peer
			DataInputStream dataInputStream = new DataInputStream(peerSocket.getInputStream());
			
			String receivedFileName = dataInputStream.readUTF();
			OutputStream outputStream = new FileOutputStream((fileDir + receivedFileName));
            
			long fileSize = dataInputStream.readLong();
			int bytesRead = 0; 
			byte[] dataBuffer = new byte[BUFF_SIZE];
			
			// Writing data to file
			while (fileSize > 0 && (bytesRead = dataInputStream.read(dataBuffer, 0, (int) Math.min(dataBuffer.length, fileSize))) != -1){
				outputStream.write(dataBuffer, 0 , bytesRead);
				fileSize = fileSize - bytesRead;
			}
			
			outputStream.close();
			System.out.println("\nFile Downloaded Successfully !!\n");
			dataInputStream.close();
			peerSocket.close();
		} catch (UnknownHostException e) {
			System.out.println(e.getMessage());
		} catch (IOException e) {
			System.out.println(e.getMessage());
		}
	}
}
