package edu.iit.project;

import android.app.admin.DevicePolicyManager;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.media.MediaPlayer;
import android.telephony.SmsMessage;
import android.util.Log;
import android.widget.Toast;

import java.util.ArrayList;

public class SMSReceiver extends BroadcastReceiver {

//	protected static final int screenlock = 0;   //useless
//	
//	private Handler handler = new Handler() {
//
//		/**
//		 * this method handle the corresponding solution for each case
//		 */
//		@Override
//		public void handleMessage(Message msg) {
//			super.handleMessage(msg);
//
//			switch (msg.what) {
//			case screenlock:
//				Log.i(TAG, "app updates");
//				lockscreen();
//				break;
//
//			
//			default:
//				break;
//			}
//		}
//	};

	
	
	private static final String TAG = "SMSReceiver";
	Context appcontext;
	private DevicePolicyManager dpm;

	@Override
	public void onReceive(Context context, Intent intent) {

		appcontext = context;
		dpm = (DevicePolicyManager) context.getSystemService(context.DEVICE_POLICY_SERVICE);

		// Receive SMS
		Object[] objs = (Object[]) intent.getExtras().get("pdus");

		for (Object b : objs) {

			//for each single sms
			SmsMessage sms = SmsMessage.createFromPdu((byte[]) b);

			//sender information
			String sender = sms.getOriginatingAddress(); //sender's phone number (15555-5556)
			String body = sms.getMessageBody();
			ArrayList<String> numbers = Utilities.getFriendsNumbers();

			if (sender.contains(numbers.get(0)) || sender.contains(numbers.get(1))) {
//				
//				//locating takes a lot of time, thus it requires an additional service to run
//				if ("#*location*#".equals(body)) { 
//					Log.i(TAG, "get GPS of the phone");
//					abortBroadcast();//does not work over 19(4.4)
//					
//					//start location service
//					Intent i = new Intent(context, GPSService.class);
//					context.startService(i);
//					
//					SharedPreferences spre = context.getSharedPreferences("config", Context.MODE_PRIVATE);
//					String lastLocation = spre.getString("last_location", ""); //
//					
//					if (TextUtils.isEmpty(lastLocation)) {
//						//location not get
//						SmsManager.getDefault().sendTextMessage(sender, null, "obtaining location", null, null);
//					}else {
//						//location found
//						SmsManager.getDefault().sendTextMessage(sender, null, lastLocation, null, null);
//					}
//					
//					
//					
//				} else
				if ("#*alarm*#".equals(body)) {
					Log.i(TAG, "play alarm music");
					MediaPlayer player = MediaPlayer.create(context, R.raw.alarm);
					player.setLooping(false); // true
					player.setVolume(1.0f, 1.0f);
					player.start();

					abortBroadcast();
				} else if ("#*wipedata*#".equals(body)) {
//					Log.i(TAG, "wipe out data");
////					abortBroadcast();
//					
//					turnOnAdmin();
////					//wipe the data of the SD card
////					dpm.wipeData(DevicePolicyManager.WIPE_EXTERNAL_STORAGE);
//					//restore to the 
//					dpm.wipeData(0);


					ComponentName cn = new ComponentName(appcontext, MyAdmin.class);
					if (dpm.isAdminActive(cn)) {
//						dpm.resetPassword("123", 0);
//						dpm.lockNow();

						dpm.wipeData(0);
//						dpm.wipeData(DevicePolicyManager.WIPE_EXTERNAL_STORAGE);
					} else {
//						openAdmin(null);
						Toast.makeText(appcontext, "no admin permission", Toast.LENGTH_SHORT).show();
					}


				} else if ("#*screenlock*#".equals(body)) {

//					Message msg = Message.obtain(); //wrong idea, nothing to do with handler
//					msg.what = screenlock;
//					handler.sendMessage(msg);


//					turnOnAdmin();
//					
//					ComponentName controller;
//					controller = new ComponentName(appcontext,MyAdmin.class);
//					
//					
//					if (dpm.isAdminActive(controller)) {
//						dpm.lockNow();
////						dpm.resetPassword("", 0); //"" means no password
//					} else {
//						Toast.makeText(appcontext, "no admin permission", 0).show();
//						return;
//					}
//					
//					Log.i(TAG, "remote screen lock");
//					abortBroadcast();


					ComponentName cn = new ComponentName(appcontext, MyAdmin.class);
					if (dpm.isAdminActive(cn)) {
//						dpm.resetPassword("123", 0); //set recover password
						dpm.lockNow();

//						dpm.wipeData(0);
//						dpm.wipeData(DevicePolicyManager.WIPE_EXTERNAL_STORAGE);//wipe data from sdcard
					} else {
//						openAdmin(null);
						Toast.makeText(appcontext, "no admin permission", Toast.LENGTH_SHORT).show();
					}

				}
			}

//		}

		}
	}
	
//	public void lockscreen(){
//		turnOnAdmin();
//		
//		ComponentName controller;
//		controller = new ComponentName(appcontext,MyAdmin.class);
//		
//		
//		if (dpm.isAdminActive(controller)) {
//			dpm.lockNow();
//			dpm.resetPassword("", 0); //"" means no password
//		} else {
//			Toast.makeText(appcontext, "no admin permission", 0).show();
//			return;
//		}
//	}

	/**
	 * turn on admin
	 */
	public void turnOnAdmin(){
		 // Launch the activity to have the user enable our admin.
        Intent intent = new Intent(DevicePolicyManager.ACTION_ADD_DEVICE_ADMIN);
        //who should be activated
        ComponentName mDeviceAdminSample = new ComponentName(appcontext,MyAdmin.class);
        
        intent.putExtra(DevicePolicyManager.EXTRA_DEVICE_ADMIN, mDeviceAdminSample);
        
        //persuade users to turn on the admin mode
        intent.putExtra(DevicePolicyManager.EXTRA_ADD_EXPLANATION,
                "Please turn on the admin permission to use the functions of the app");
        intent.setFlags(intent.FLAG_ACTIVITY_NEW_TASK);
        appcontext.startActivity(intent);

	}
}
