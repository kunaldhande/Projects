package edu.iit.project;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.admin.DevicePolicyManager;
import android.content.ComponentName;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;
import android.view.MenuItem;
import android.view.View;
import android.widget.AutoCompleteTextView;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.Switch;

import com.parse.LogInCallback;
import com.parse.ParseException;
import com.parse.ParseUser;
import com.parse.RequestPasswordResetCallback;

import java.util.ArrayList;

public class SettingActivity extends AppCompatActivity {

    private DevicePolicyManager dpm;
    Button uninstallBtn;
    Switch swichAdmin;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);

        setTitle("Settings");
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        uninstallBtn = (Button) findViewById(R.id.uninstallBtn);
        swichAdmin = (Switch) findViewById(R.id.adminSwitch);

        if (swichAdmin != null) {
            Boolean isDeviceAdminEnabled = Utilities.getSharedPrefBool("DEVICEADMIN");
            swichAdmin.setChecked(isDeviceAdminEnabled);
            uninstallBtn.setVisibility(isDeviceAdminEnabled ? View.VISIBLE : View.INVISIBLE);
            swichAdmin.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                    if(isChecked) {
                        enableAdmin();
                    }
                    else {
                        disableAdmin();
                    }
                }
            });
        }

        dpm = (DevicePolicyManager) getSystemService(DEVICE_POLICY_SERVICE);
    }


    void changeOption(Boolean isChecked) {
        String currentDevice = Utilities.android_id + ":~-~:" + Build.MODEL;
        ArrayList<String> devices = (ArrayList<String>) ParseUser.getCurrentUser().get("settings");
        for(int i=0; i<devices.size(); i++) {
            if(devices.get(i).contains(currentDevice)) {
                devices.set(i, currentDevice + ":~-~:" + isChecked);
                break;
            }
        }

        if(isChecked) {
            Utilities.saveToSharedPref("DEVICEADMIN", true);
            uninstallBtn.setVisibility(View.VISIBLE);
        }
        else {
            Utilities.saveToSharedPref("DEVICEADMIN", false);
            uninstallBtn.setVisibility(View.INVISIBLE);
            swichAdmin.setChecked(false);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {

        if (requestCode == 100) {
            if(resultCode == RESULT_OK){
                changeOption(true);
            }
            if (resultCode == RESULT_CANCELED) {
                changeOption(false);
            }
        }
    }//onActivityResult

    // below are jingtian code
    public void enableAdmin(){
        //turn on super admin
        Intent intent = new Intent(DevicePolicyManager.ACTION_ADD_DEVICE_ADMIN);
        ComponentName cn = new ComponentName(this, MyAdmin.class);
        intent.putExtra(DevicePolicyManager.EXTRA_DEVICE_ADMIN, cn);
        //persuade user to turn on
        intent.putExtra(DevicePolicyManager.EXTRA_ADD_EXPLANATION,
                "confirm to turn on anti-theft function");
        startActivityForResult(intent,100);
    }

    public void disableAdmin() {
        ComponentName cn = new ComponentName(this, MyAdmin.class);
        //delete admin and uninstall app
        dpm.removeActiveAdmin(cn);
        changeOption(false);
    }

    public void uninstall(View view){
        final Dialog dialog = new Dialog(this);
        dialog.setContentView(R.layout.confirm_password);
        dialog.setTitle("Confirm Password");


        Button canclelBtn = (Button) dialog.findViewById(R.id.cancelBtn);
        // if button is clicked, close the custom dialog
        canclelBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialog.dismiss();
            }
        });

        Button done = (Button) dialog.findViewById(R.id.doneBtn);
        done.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                final AutoCompleteTextView passwordView =  (AutoCompleteTextView) dialog.findViewById(R.id.password);
                String password = passwordView.getText().toString();
                // Check for a valid email address.
                if (TextUtils.isEmpty(password)) {
                    passwordView.setError(getString(R.string.error_field_required));
                    passwordView.requestFocus();
                    return;
                }

                ParseUser.logInInBackground(ParseUser.getCurrentUser().getUsername(), password,
                        new LogInCallback() {
                            public void done(ParseUser user, ParseException e) {
                                if (user != null) {
                                    // Save Current session to Shared Prefrences
                                    Utilities.saveToSharedPref("userSessionToken", ParseUser.getCurrentUser().getSessionToken());

                                    disableAdmin();
                                    Intent intent = new Intent();
                                    intent.setAction("android.intent.action.UNINSTALL_PACKAGE");
                                    intent.addCategory("android.intent.category.DEFAULT");
                                    intent.setData(Uri.parse("package:" + getPackageName()));
                                    startActivity(intent);
                                } else {
                                    passwordView.setError(getString(R.string.error_incorrect_password));
                                    passwordView.requestFocus();
                                }
                            }
                        });
            }
        });

        dialog.show();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                finish();
                return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
