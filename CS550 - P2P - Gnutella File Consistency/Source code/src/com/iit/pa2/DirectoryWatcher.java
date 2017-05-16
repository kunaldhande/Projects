package com.iit.pa2;

import java.io.IOException;
import java.nio.file.FileSystems;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.WatchEvent;
import java.nio.file.WatchKey;
import java.nio.file.WatchService;

import static java.nio.file.StandardWatchEventKinds.ENTRY_DELETE;
import static java.nio.file.StandardWatchEventKinds.ENTRY_MODIFY;
import static java.nio.file.StandardWatchEventKinds.ENTRY_CREATE;

public class DirectoryWatcher implements Runnable{
	WatchService watcher;
	Peer client;
	
	/**
	 * parameterized constructor
	 * Initializes directory watcher
	 * @param client 
	 * @param fileDir
	 * @param client
	 * @param fileDir 
	 * @throws IOException
	 */
	public DirectoryWatcher(Peer client, String fileDir) throws IOException{
		this.client = client;
		this.watcher = FileSystems.getDefault().newWatchService();
		Path dir = Paths.get(fileDir);
		dir.register(watcher, ENTRY_CREATE, ENTRY_DELETE, ENTRY_MODIFY);
	}
	
	
	/**
	 * Method to trigger method to invalidate if file is modified
	 * @param fileName
	 */
	private void triggerMsg(String fileName) {
		for (FileInfo f : client.localFiles) {
    		if(f.fileName.equals(fileName)) {
    			if (client.isPush) {
	    			f.versionIncrease();
	    			Thread threadInstance = new Thread(new NeighborHandler(client.getMsgID(), client.peerDetails.hostIP, client.peerDetails.peerID, client.peerDetails.portNo, fileName.toString(), f.versionNum, "invalidate"));
	    			threadInstance.start();
	    			break;
    			} else {
    				f.lastModifiedTime = System.currentTimeMillis();
    			}
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
		        if (kind == ENTRY_CREATE) {
		        }
		        else if (kind == ENTRY_DELETE) {
		        	triggerMsg(fileName.toString());
		        }
		        else if (kind == ENTRY_MODIFY) {
		        	triggerMsg(fileName.toString());
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
