package edu.iit.project;

import android.Manifest;
import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.telephony.TelephonyManager;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewManager;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.parse.ParseException;
import com.parse.ParseUser;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashSet;

import mehdi.sakout.fancybuttons.FancyButton;

public class MainMenu extends AppCompatActivity {

    boolean isGuest;
    ArrayList<String> devices;
    ArrayList<String> deviceSetting;

    @Override
    @SuppressWarnings("unchecked")
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_menu);

        isGuest = getIntent().getBooleanExtra("GUESTUSER", false);
        String name = ParseUser.getCurrentUser().get("name").toString();
        Toast.makeText(this, isGuest ? "You loggedIn as a Guest : " + name : "You are LoggedIn as " + name , Toast.LENGTH_SHORT).show();

        setTitle("MobiSecure" + (isGuest ? " : " + name : ""));

        View header = getLayoutInflater().inflate(R.layout.list_header, null);
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);
        int btnSize =  (int)(metrics.widthPixels / 2 * 0.8);

        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(btnSize, Utilities.dpToPx(50));
        params.rightMargin = (int)(metrics.widthPixels / 2 * 0.025);
        params.leftMargin = (int)(metrics.widthPixels / 2 * 0.025);

        FancyButton profileBtn = (FancyButton) header.findViewById(R.id.btn_profile);
        profileBtn.setLayoutParams(params);

        FancyButton friendsBtn = (FancyButton) header.findViewById(R.id.btn_friends);
        friendsBtn.setLayoutParams(params);

        FancyButton virusBtn = (FancyButton) header.findViewById(R.id.btn_virusCheck);
        virusBtn.setLayoutParams(params);
        if(isGuest)
            ((ViewManager)virusBtn.getParent()).removeView(virusBtn);

        FancyButton logoutBtn = (FancyButton) header.findViewById(R.id.btn_logout);
        logoutBtn.setLayoutParams(params);

        if(isGuest)
            logoutBtn.setText("Logout");
        else
            logoutBtn.setText("Login As Guest");

        logoutBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(isGuest)
                    logOut();
                else
                    signInAsOtherUser();
            }
        });


        try {
            ParseUser.getCurrentUser().fetch();
            devices = (ArrayList<String>) ParseUser.getCurrentUser().get("devices");
            deviceSetting = (ArrayList<String>) ParseUser.getCurrentUser().get("settings");
            if (devices == null) {
                devices = new ArrayList<String>();
            }

            if (deviceSetting == null) {
                deviceSetting = new ArrayList<String>();
            }

            if (!isGuest) {
                // Save Current session to Shared Prefrences
                Utilities.saveToSharedPref("userSessionToken", ParseUser.getCurrentUser().getSessionToken());

                // Here, thisActivity is the current activity
                if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_PHONE_STATE)
                        != PackageManager.PERMISSION_GRANTED) {

                    // Should we show an explanation?
                    if (ActivityCompat.shouldShowRequestPermissionRationale(this,
                            Manifest.permission.READ_PHONE_STATE) && ActivityCompat.shouldShowRequestPermissionRationale(this,
                            Manifest.permission.SEND_SMS)) {

                        saveDeviceData();

                    } else {

                        // No explanation needed, we can request the permission.

                        ActivityCompat.requestPermissions(this,
                                new String[]{Manifest.permission.READ_PHONE_STATE, Manifest.permission.SEND_SMS}, 100);

                        // MY_PERMISSIONS_REQUEST_READ_CONTACTS is an
                        // app-defined int constant. The callback method gets the
                        // result of the request.
                    }
                }
                else {
                    saveDeviceData();
                }

                // Check for Location
                MyApp.checkForLocation();

                // Alarm
                AlarmManager alarmManager = (AlarmManager) getBaseContext().getSystemService(Context.ALARM_SERVICE);
                Intent intent = new Intent(getBaseContext(), LocationTrackerReceiver.class);
                PendingIntent pendingIntent = PendingIntent.getBroadcast(getBaseContext(), 0, intent, 0);
                alarmManager.setRepeating(AlarmManager.RTC_WAKEUP, System.currentTimeMillis(), 3600000, pendingIntent);
            }
        } catch (ParseException e) {
            e.printStackTrace();
        }

        MenuListAdapter adapter = new MenuListAdapter(MainMenu.this);
        ListView list = (ListView) findViewById((R.id.menuView));
        list.addHeaderView(header);
        if (list != null) {
            list.setAdapter(adapter);
        }

        //copy the files under resource folder to the internal storage
        copyDB("antivirus.db");
    }

    void saveDeviceData () {
        TelephonyManager telephoneMgr = (TelephonyManager) getBaseContext().getSystemService(Context.TELEPHONY_SERVICE);
        devices.add(Utilities.android_id + ":~-~:" + Build.MODEL + ":~-~:" + telephoneMgr.getLine1Number());
        devices = new ArrayList<String>(new HashSet<String>(devices));
        ParseUser.getCurrentUser().put("devices", devices);

        deviceSetting.add(Utilities.android_id + ":~-~:" + Build.MODEL + ":~-~:" + String.valueOf(false));
        deviceSetting = new ArrayList<String>(new HashSet<String>(deviceSetting));
        ParseUser.getCurrentUser().put("settings", deviceSetting);

        try {
            ParseUser.getCurrentUser().save();
        } catch (ParseException e) {
            e.printStackTrace();
        }

        Utilities.getPhoneAndSMS();
    }
    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
        switch (requestCode) {
            case 100: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {

                   saveDeviceData();

                } else {

                    // permission denied, boo! Disable the
                    // functionality that depends on this permission.
                }
                return;
            }

            // other 'case' lines to check for other
            // permissions this app might request
        }
    }


    @Override
    public void onBackPressed() {
        super.onBackPressed();
        if(isGuest) {
            Toast.makeText(this, "You are LoggedOut as Guest" , Toast.LENGTH_SHORT).show();
            logOut();
        }
    }

    public void showProfile(View v) {
        Intent intent = new Intent(MainMenu.this, ProfileActivity.class);
        intent.putExtra("GUESTUSER", true);
        startActivity(intent); //
    }

    public void showFriends(View v) {
        Intent intent = new Intent(MainMenu.this, FriendActivity.class);
        intent.putExtra("GUESTUSER", true);
        startActivity(intent);
    }

    public void showSettings() {
        Intent intent = new Intent(MainMenu.this, SettingActivity.class);
        intent.putExtra("GUESTUSER", true);
        startActivity(intent);
    }

    public void logOut() {
        ParseUser.becomeInBackground(Utilities.getSharedPref("userSessionToken"));
        ParseUser user = ParseUser.getCurrentUser();
        finish();
    }

    public void signInAsOtherUser () {
        Intent intent = new Intent(this, LoginActivity.class);
        intent.putExtra("GUESTUSER", true);
        startActivity(intent);
    }

    private class MenuListAdapter extends ArrayAdapter<String> {

        Context contect;

        public MenuListAdapter(Context context) {
            super(MainMenu.this, R.layout.listview, devices);
            this.contect = context;
        }

        @Override
        public View getView(final int position, View itemView, ViewGroup parent) {

            if (itemView == null) {
                itemView = getLayoutInflater().inflate(R.layout.listview, parent, false);
            }

            final String deviceName = devices.get(position).split(":~-~:")[1];


            TextView item = (TextView) itemView.findViewById(R.id.itemName);
            item.setText(deviceName);
            if(devices.get(position).contains(Utilities.android_id + ":~-~:" + Build.MODEL)) {
                item.setText("* " + deviceName);
                item.setTextColor(Color.parseColor("#7fb17f"));
            }
            else
                item.setTextColor(Color.BLACK);

            itemView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Intent intent = new Intent(contect, MapsActivity.class);
                    intent.putExtra("SELECTED", devices.get(position));
                    startActivity(intent);
                }
            });

            final View finalItemView = itemView;
            itemView.setOnLongClickListener(new View.OnLongClickListener() {
                @Override
                public boolean onLongClick(View v) {
                    boolean isAdminEnabled = false;
                    CharSequence colors[];
                    for(int i = 0; i < deviceSetting.size(); i++)
                        if(deviceSetting.get(i).equals(devices.get(position))) {
                            isAdminEnabled = Boolean.valueOf(deviceSetting.get(i).split(":~-~:")[2]);
                            break;
                        }

                    if(!devices.get(position).contains(Utilities.android_id + ":~-~:" + Build.MODEL) || isAdminEnabled)
                        colors = new CharSequence[] {"Show Recent Visits", "Play Ringtone", "Lock Device", "Wipe Data"};
                    else
                        colors = new CharSequence[] {"Show Recent Visits"};

                    AlertDialog.Builder builder = new AlertDialog.Builder(contect);
                    builder.setTitle(deviceName);
                    builder.setItems(colors, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            if(which == 0)
                                finalItemView.callOnClick();

                            else {
                                String deviceList[] = devices.get(position).split(":~-~:");
                                System.out.println("DeviceL " + deviceList);
                                ArrayList<String> numbers = new ArrayList<String>();
                                if(deviceList.length == 3) {
                                    numbers.add(deviceList[2]);
                                    numbers.add("");
                                }
                                else {
                                    Toast.makeText(contect, "No phone number registered with this Device", Toast.LENGTH_LONG).show();
                                    return;
                                }

                                if(which == 1)
                                    Utilities.sendMessage("#*alarm*#", numbers);

                                else if(which == 2)
                                    Utilities.sendMessage("#*screenlock*#", numbers);

                                else if(which == 3)
                                    Utilities.sendMessage("#*wipedata*#", numbers);
                            }
                        }
                    });
                    builder.show();
                    return false;
                }
            });

            return itemView;
        }
    }

    public void onClickVirus(View view){
        Intent intent = new Intent(this, VirusKillerActivity.class);
        startActivity(intent);
    }

    /**
     * copy the database to data/data/<package>/files/address.db
     */
    private void copyDB(String filename) {
        try {
            File file = new File(getFilesDir(),filename);
            if (file.exists() && file.length() > 0) {
                //file exists, no need to copy
                Log.i("Lock", "file exists, no need to copy");
            }else {
                InputStream is = getAssets().open(filename);

                FileOutputStream fos = new FileOutputStream(file);
                byte[] buffer = new byte[1024];
                int length = 0;
                while ((length = is.read(buffer)) != -1) {
                    fos.write(buffer,0,length);
                }
                is.close();
                fos.close();
            }

        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    public boolean onCreateOptionsMenu(Menu menu) {
        if(!isGuest)
            getMenuInflater().inflate(R.menu.menu_home, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        //handle presses on the action bar items
        switch (item.getItemId()) {

            case R.id.action_settings:
                showSettings();
                return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
