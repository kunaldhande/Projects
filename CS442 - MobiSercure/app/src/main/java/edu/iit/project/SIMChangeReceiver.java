package edu.iit.project;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class SIMChangeReceiver extends BroadcastReceiver {
    public SIMChangeReceiver() {
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        Utilities.getPhoneAndSMS();
    }
}
