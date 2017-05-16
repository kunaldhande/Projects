package com.iit.pa2;
import java.io.Serializable;


/**
 * Class to maintain the peer details like PeerID, portNo, hostIP
 * @serial serialVersionUID
 */
public class PeerDetails implements Serializable {
	private static final long serialVersionUID = 1L;
	
	public int peerID;
	public int portNo;
	public String hostIP;
}
