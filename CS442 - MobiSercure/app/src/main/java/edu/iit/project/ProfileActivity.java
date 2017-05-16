package edu.iit.project;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.KeyEvent;
import android.view.MenuItem;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.EditText;
import android.widget.TextView;

import com.parse.ParseUser;

public class ProfileActivity extends AppCompatActivity {

    EditText nameView;
    EditText emailView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_profile);

        setTitle("Profile");
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        nameView = (EditText) findViewById(R.id.name);
        emailView = (EditText) findViewById(R.id.email);

        ParseUser user = ParseUser.getCurrentUser();
        if(user.get("name") != null)
            nameView.setText(user.get("name").toString());

        emailView.setText(user.getUsername());
        emailView.setEnabled(false);

        nameView.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                if (actionId == EditorInfo.IME_ACTION_DONE) {
                    ParseUser user = ParseUser.getCurrentUser();
                    user.put("name", nameView.getText().toString());
                    user.saveInBackground();
                }
                return false;
            }
        });
    }

    public void onChangePassword (View v) {
        Intent intent = new Intent(this, ResetPassword.class);
        startActivity(intent);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                ParseUser user = ParseUser.getCurrentUser();
                user.put("name", nameView.getText().toString());
                user.saveInBackground();
                finish();
                return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
