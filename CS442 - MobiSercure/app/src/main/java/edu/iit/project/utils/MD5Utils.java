package edu.iit.project.utils;

import java.io.File;
import java.io.FileInputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class MD5Utils {

	/**
	 * using md5 algorithm to encrypt password
	 * @param password
	 * @return
	 */
	public static String encrypt(String password) {

		try {
			
			MessageDigest digest = MessageDigest.getInstance("md5");
			byte[] result = digest.digest(password.getBytes());
			StringBuffer buffer = new StringBuffer();
			
			for (byte b : result) {
				
				int number = b & 0xff;
				String str = Integer.toHexString(number);
				// System.out.println(str);
				if (str.length() == 1) {
					buffer.append("0");
				}
				buffer.append(str);
			}

			return buffer.toString();
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
			return "";
		}

	}
	
	
	/**
	 * obtain file's digest
	 * @param filePath
	 * @return
	 */
	public static String getFileDigest(String filePath) {
		
		try {
			MessageDigest digest = MessageDigest.getInstance("md5"); //designate which method to use
			File file = new File(filePath);
			FileInputStream fis = new FileInputStream(file);
			byte[] buffer = new byte[1024];
			int length = -1;
			while((length = fis.read(buffer)) != -1){
				digest.update(buffer, 0, length);
			}
			byte[] result = digest.digest();
			
			StringBuffer sb = new StringBuffer();
			for (byte b : result) {
				int number = b & 0xff;
				String str = Integer.toHexString(number);
				if (str.length() == 1) {
					sb.append("0");
				}
				sb.append(str);
			}
			return sb.toString();
		} catch (Exception e) {
			e.printStackTrace();
			return ""; //we have written the digest method manually, no error will happen
		}
		
		
	}
}
