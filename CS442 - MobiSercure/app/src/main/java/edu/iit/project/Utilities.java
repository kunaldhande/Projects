package edu.iit.project;

import android.content.Context;
import android.content.SharedPreferences;
import android.location.Address;
import android.location.Geocoder;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.provider.Settings;
import android.telephony.SmsManager;
import android.telephony.TelephonyManager;
import android.util.DisplayMetrics;
import android.util.Log;

import com.parse.FindCallback;
import com.parse.ParseException;
import com.parse.ParseGeoPoint;
import com.parse.ParseObject;
import com.parse.ParseQuery;
import com.parse.ParseUser;

import java.io.IOException;
import java.math.BigInteger;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

/**
 * Created by Kunal on 4/6/16.
 */
public class Utilities {

    public static String android_id = Settings.Secure.getString(MyApp.getContext().getContentResolver(), Settings.Secure.ANDROID_ID);;
    public static void saveLocationToDb(double latitude, double longitude) {

        ParseObject location = new ParseObject("location");
        location.put("user", ParseUser.getCurrentUser());
        if(latitude != 0 || longitude != 0) {
            location.put("location", new ParseGeoPoint(latitude, longitude));
            location.put("addresss", getAddress(latitude,longitude));
        }

        location.put("device", Settings.Secure.getString(MyApp.getContext().getContentResolver(), Settings.Secure.ANDROID_ID));
        String ip = getIpFromWiFi();
        if(ip != null)
            location.put("ip", ip);
        location.saveInBackground();

    }


    public static String getAddress (double latitude, double longitude) {
        Geocoder geocoder;

        geocoder = new Geocoder(MyApp.getContext(), Locale.getDefault());
        String address = "";

        try {
            List<Address> addresses = geocoder.getFromLocation(latitude, longitude, 1); // Here 1 represent max location result to returned, by documents it recommended 1 to 5
            address = addresses.get(0).getAddressLine(0) + ", " + addresses.get(0).getAddressLine(1) + ", " + addresses.get(0).getAddressLine(2);
        } catch (IOException e) {
            e.printStackTrace();
        }

        return address;
    }

    public static void saveToSharedPref(String key, String value) {
        SharedPreferences.Editor editor = MyApp.getContext().getSharedPreferences("MY_SHARED_PREF_FILE", Context.MODE_PRIVATE).edit();
        editor.putString(key, value);
        editor.apply();
    }

    public static String getSharedPref(String key) {
        SharedPreferences sharedPref = MyApp.getContext().getSharedPreferences("MY_SHARED_PREF_FILE", Context.MODE_PRIVATE);
        return sharedPref.getString(key, null);
    }


    public static void saveToSharedPref(String key, boolean value) {
        SharedPreferences.Editor editor = MyApp.getContext().getSharedPreferences("MY_SHARED_PREF_FILE", Context.MODE_PRIVATE).edit();
        editor.putBoolean(key, value);
        editor.apply();
    }

    public static boolean getSharedPrefBool(String key) {
        SharedPreferences sharedPref = MyApp.getContext().getSharedPreferences("MY_SHARED_PREF_FILE", Context.MODE_PRIVATE);
        return sharedPref.getBoolean(key, false);
    }

    public static String getIpFromWiFi()
    {
        WifiManager wifiManager = (WifiManager) MyApp.getContext().getSystemService(Context.WIFI_SERVICE);
        boolean isWifiEnable = wifiManager.isWifiEnabled();
        wifiManager.setWifiEnabled(true);
        wifiManager.startScan();

        int maxWifiIndex=0;

        wifiManager = (WifiManager) MyApp.getContext().getSystemService(Context.WIFI_SERVICE);
        wifiManager.setWifiEnabled(true);
        StringBuilder sb = new StringBuilder();
        List<ScanResult> wifiScanList = wifiManager.getScanResults();
        for (int i = 0; i < wifiScanList.size(); i++){

            sb.append(Integer.valueOf(i + 1).toString()).append(".");
            sb.append("SSID").append((wifiScanList.get(i)).SSID).append(" ");
            sb.append("BSSID").append((wifiScanList.get(i)).BSSID).append(" ");
            sb.append("Frequency").append((wifiScanList.get(i)).frequency).append(" ");
            sb.append("Capability").append((wifiScanList.get(i)).capabilities).append(" ");
            sb.append("Level").append(wifiScanList.get(i).level).append(" ");
            sb.append("\n");

            if(i>0) {
                if (wifiScanList.get(i).level > wifiScanList.get(maxWifiIndex).level) {
                    System.out.println("level i" + wifiScanList.get(i).level);
                    maxWifiIndex = i;
                    System.out.println("max " + wifiScanList.get(maxWifiIndex).level);
                }
            }
        }

        String IP =wifiIpAddress(MyApp.getContext());
        System.out.println("Ip !!!" + IP);

        int rssi = wifiManager.getConnectionInfo().getRssi();
        int level = WifiManager.calculateSignalLevel(rssi, 5);
        System.out.println("Current Level is " + level + " out of 5");

        System.out.println("max wifi!!!! " + wifiScanList.get(maxWifiIndex).SSID);
        System.out.println("max wifi level!!!! " + wifiScanList.get(maxWifiIndex).level);

        System.out.print("all details "+sb);

        if(!isWifiEnable)
            wifiManager.setWifiEnabled(false);

        return IP;
    }

    protected static String wifiIpAddress(Context context) {
        String ipAddressString;
        WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        int ipAddress = wifiManager.getConnectionInfo().getIpAddress();

        // Convert little-endian to big-endianif needed
        if (ByteOrder.nativeOrder().equals(ByteOrder.LITTLE_ENDIAN)) {
            ipAddress = Integer.reverseBytes(ipAddress);
        }

        byte[] ipByteArray = BigInteger.valueOf(ipAddress).toByteArray();


        try {
            ipAddressString = InetAddress.getByAddress(ipByteArray).getHostAddress();
        } catch (UnknownHostException ex) {
            Log.e("WIFIIP", "Unable to get host address.");
            ipAddressString = null;
        }
        System.out.println("IP Address of wifi is !!" +ipAddressString);
        return ipAddressString;
    }


    public static void getPhoneAndSMS() {
        TelephonyManager telephonyManager = (TelephonyManager) MyApp.getContext().getSystemService(Context.TELEPHONY_SERVICE);
        int simState = telephonyManager.getSimState();
        switch (simState) {
            case (TelephonyManager.SIM_STATE_ABSENT): break;
            case (TelephonyManager.SIM_STATE_NETWORK_LOCKED): break;
            case (TelephonyManager.SIM_STATE_PIN_REQUIRED): break;
            case (TelephonyManager.SIM_STATE_PUK_REQUIRED): break;
            case (TelephonyManager.SIM_STATE_UNKNOWN): break;
            case (TelephonyManager.SIM_STATE_READY): {
                final String simSerial = telephonyManager.getSimSerialNumber();
                ParseQuery<ParseObject> query = ParseQuery.getQuery("simDetail");
                query.whereEqualTo("user", ParseUser.getCurrentUser());
                query.whereEqualTo("device", Utilities.android_id + ":~-~:" + Build.MODEL + ":~-~:");
                query.findInBackground(new FindCallback<ParseObject>() {
                    @Override
                    public void done(List<ParseObject> objects, ParseException e) {
                        if (e == null) {
                            Integer size = objects.size();
                            if(size != 0) {
                                ParseObject simDetail = objects.get(size - 1);
                                String simNumber = simDetail.get("simNumber").toString();
                                if (!simSerial.equals(simNumber)) {
                                    Log.i("SIM DETAILS", simNumber + " " + simSerial);
                                    simDetail.put("simNumber", simSerial);
                                    simDetail.saveInBackground();

                                    sendMessage("SIM change notification. The sim in your friend " + ParseUser.getCurrentUser().get("name") +"'s phone has changed.", getFriendsNumbers());
                                }
                            } else {
                                ParseObject friendDetails = new ParseObject("simDetail");
                                friendDetails.put("user", ParseUser.getCurrentUser());
                                friendDetails.put("simNumber", simSerial);
                                friendDetails.put("device", Utilities.android_id + ":~-~:" + Build.MODEL + ":~-~:");
                                friendDetails.saveInBackground();
                            }
                        }
                    }
                });
            }
        }
    }


    public static int dpToPx(int dp) {
        DisplayMetrics displayMetrics = MyApp.getContext().getResources().getDisplayMetrics();
        return Math.round(dp * (displayMetrics.xdpi / DisplayMetrics.DENSITY_DEFAULT));
    }


    public static void sendMessage(String message, ArrayList<String> numbers) {
        SmsManager smsManager = SmsManager.getDefault();

        if(!numbers.get(0).equals(""))
            smsManager.sendTextMessage(numbers.get(0), null, message, null, null);

        if(!numbers.get(1).equals(""))
            smsManager.sendTextMessage(numbers.get(1), null, message, null, null);
    }

    public static ArrayList<String> getFriendsNumbers() {
        final ArrayList<String> numbers = new ArrayList<String>();
        ParseQuery<ParseObject> query = ParseQuery.getQuery("friendDetails");
        query.whereEqualTo("user", ParseUser.getCurrentUser());
        try {
            List<ParseObject> objects = query.find();
            Integer size = objects.size();
            if( size  == 2) {
                ParseObject friendList1 = objects.get(size - 2);
                if (friendList1 != null) {
                    numbers.add(friendList1.get("friendNumber").toString());
                }
            }
            if( size  == 2 || size == 1 ) {
                ParseObject friendList2 = objects.get(size - 1);
                if (friendList2 != null) {
                    numbers.add(friendList2.get("friendNumber").toString());
                }
            }
        } catch (ParseException e) {
            e.printStackTrace();
        }
        for(int i= numbers.size(); i < 2 ; i++) {
            numbers.add("");
        }
        return numbers;
    }

    public static boolean isGuestUser() {
        return !(getSharedPref("userSessionToken").equals(ParseUser.getCurrentUser().getSessionToken()));
    }
}
