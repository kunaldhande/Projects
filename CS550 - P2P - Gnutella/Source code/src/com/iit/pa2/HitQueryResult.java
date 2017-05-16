package com.iit.pa2;

import java.io.Serializable;
import java.util.ArrayList;

/**
 * Class to store result of hitQuery
 * @serial serialVersionUID
 */
public class HitQueryResult implements Serializable {
	private static final long serialVersionUID = 1L;
	
	public ArrayList<String> paths = new ArrayList<String>();
	public ArrayList<PeerDetails> searchResults = new ArrayList<PeerDetails>();
}
