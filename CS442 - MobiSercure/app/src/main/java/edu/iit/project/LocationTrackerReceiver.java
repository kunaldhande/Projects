package edu.iit.project;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class LocationTrackerReceiver extends BroadcastReceiver {
    public LocationTrackerReceiver() {
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        // TODO: This method is called when the BroadcastReceiver is receiving
        // an Intent broadcast.
        Log.d("","");
        MyApp.checkForLocation();
    }
}
