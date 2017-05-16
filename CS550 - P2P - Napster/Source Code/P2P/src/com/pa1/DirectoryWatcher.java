package com.pa1;

import java.io.File;
import java.io.IOException;
import java.nio.file.FileSystems;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.WatchEvent;
import java.nio.file.WatchKey;
import java.nio.file.WatchService;
import java.util.HashMap;

import static java.nio.file.StandardWatchEventKinds.ENTRY_DELETE;
import static java.nio.file.StandardWatchEventKinds.ENTRY_MODIFY;

public class DirectoryWatcher implements Runnable{
	WatchService watcher;
	PeerClient client;
	
	/**
	 * parameterized constructor
	 * Initializes directory watcher
	 * @param fileDir
	 * @param client
	 * @throws IOException
	 */
	public DirectoryWatcher(String fileDir, PeerClient client, Boolean autoIndexingEnabled) throws IOException{
		this.client = client;
		this.watcher = FileSystems.getDefault().newWatchService();
		Path dir = Paths.get(fileDir);
		
		if (autoIndexingEnabled) {
			dir.register(watcher, ENTRY_DELETE, ENTRY_MODIFY);
			registerAllfilesinFolder();
		}
		else
			dir.register(watcher, ENTRY_DELETE);
	}
	
	private void registerAllfilesinFolder() {
		File[] files = new File(client.fileDir).listFiles();
		//If this pathname does not denote a directory, then listFiles() returns null. 

		for (File file : files) {
		    if (file.isFile()) {
		    	// Registering index
	        	HashMap<String, String> peerInfo = new HashMap<String, String>();
				peerInfo.put("peerID", P2PFileTransfer.getIPAddress());
				peerInfo.put("portNo", String.valueOf(client.peerPort));
				peerInfo.put("fileName", file.getName());
	        	peerInfo.put("operation", "register");
	        	client.registerFile(peerInfo);
		    }
		}
	}

	@Override
	public void run() {
		while (true) {
		    WatchKey key;
		    try {
		        // wait for a key to be available
		        key = watcher.take();
		    } catch (InterruptedException ex) {
		        return;
		    }
		 
		    for (WatchEvent<?> event : key.pollEvents()) {
		        // get event type
		        WatchEvent.Kind<?> kind = event.kind();
		 
		        // get file name
		        @SuppressWarnings("unchecked")
		        WatchEvent<Path> ev = (WatchEvent<Path>) event;
		        Path fileName = ev.context();
		 
		        System.out.println(kind.name() + ": " + fileName);
		 
		        if (kind == ENTRY_DELETE) {
		        	// Deleting index
		        	HashMap<String, String> peerInfo = new HashMap<String, String>();
					peerInfo.put("peerID", P2PFileTransfer.getIPAddress());
					peerInfo.put("portNo", String.valueOf(client.peerPort));
					peerInfo.put("fileName", fileName.toString());
		        	peerInfo.put("operation", "delete");
		        	client.deleteFile(peerInfo);
		 
		        }
		        else if (kind == ENTRY_MODIFY) {
		        	// Registering index
		        	HashMap<String, String> peerInfo = new HashMap<String, String>();
					peerInfo.put("peerID", P2PFileTransfer.getIPAddress());
					peerInfo.put("portNo", String.valueOf(client.peerPort));
					peerInfo.put("fileName", fileName.toString());
		        	peerInfo.put("operation", "register");
		        	client.registerFile(peerInfo);
		        }
		    }
		 
		    // IMPORTANT: The key must be reset after processed to work next watch event properly.
		    boolean valid = key.reset();
		    if (!valid) {
		        break;
		    }
		}
		
	}
}
