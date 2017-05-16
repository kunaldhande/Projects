package edu.iit.project;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import com.parse.LogInCallback;
import com.parse.ParseException;
import com.parse.ParseUser;

public class ResetPassword extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_reset_password);

        setTitle("Reset Password");
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
    }

    public void changePassword(View v) {
        final EditText oldpw= (EditText)findViewById(R.id.oldPassword);
        String oldPassword=String.valueOf(oldpw.getText().toString());


        EditText newpw= (EditText)findViewById(R.id.newPassword);
        final String newPassword=String.valueOf(newpw.getText().toString());

        EditText conpw= (EditText)findViewById(R.id.confirmPassword);
        String confirmPassword=String.valueOf(conpw.getText().toString());

        if(!newPassword.equals(confirmPassword))
        {
            newpw.setError(getString(R.string.error_match_password));
            newpw.requestFocus();
            System.out.println("new password and confirm password not same");
            return;
        }

        if(newpw.length() <= 4) {
            newpw.setError(getString(R.string.error_invalid_password));
            newpw.requestFocus();
            return;
        }

        try {
            ParseUser user = ParseUser.logIn(ParseUser.getCurrentUser().getUsername(), oldPassword);
            if (user != null) {
                user.setPassword(newPassword);
                user.save();
                user = ParseUser.logIn(ParseUser.getCurrentUser().getUsername(), newPassword);
                user.fetch();

                // Save Current session to Shared Prefrences
                if (user != null) {
                    Utilities.saveToSharedPref("userSessionToken", ParseUser.getCurrentUser().getSessionToken());
                    finish();
                    Toast.makeText(getApplicationContext(), "Password Changed successfully", Toast.LENGTH_SHORT).show();
                }
                else {
                    Toast.makeText(getApplicationContext(), "Failed to change password.", Toast.LENGTH_SHORT).show();
                }

            } else {
                oldpw.setError(getString(R.string.error_old_match_password));
                oldpw.requestFocus();
            }
        } catch (ParseException e) {
            e.printStackTrace();
        }
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
