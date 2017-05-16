package com.iit.pa2;
import java.rmi.*;


/**
 * Interface for RMI to download and search 
 */
public interface PeerInterface extends Remote {
	
	public byte[] obtain(String filename) throws RemoteException;
	public FileInfo obtainInfo(String filename) throws RemoteException;
	
	public HitQueryResult query(String filename, int fromPeerId, String msgId, int TTL) throws RemoteException;
	public void invalidation(String msgID, int originServerID, String filename, int versionNumber) throws RemoteException;
	
}
