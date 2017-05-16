package edu.iit.project;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.parse.FindCallback;
import com.parse.ParseException;
import com.parse.ParseObject;
import com.parse.ParseQuery;
import com.parse.ParseUser;

import java.util.List;


/**
 * Created by Nithin on 4/9/16.
 */
public class FriendActivity extends AppCompatActivity {
    private EditText friendName1;
    private EditText friendPhone1;
    private EditText friendName2;
    private EditText friendPhone2;
    private Button addButton;
    List<ParseObject> friendsList;
    public boolean isSignUpUser;

    protected void onCreate (Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_friend);
        setTitle("Add Friends");

        isSignUpUser = getIntent().getBooleanExtra("isSignUpUser", false);

        friendName1 = (EditText)findViewById(R.id.editText);
        friendName2 = (EditText)findViewById(R.id.editText3);
        friendPhone1 = (EditText)findViewById(R.id.editText2);
        friendPhone2 = (EditText)findViewById(R.id.editText4);
        addButton = (Button)findViewById(R.id.button);

        if(isSignUpUser)
            addButton.setVisibility(View.INVISIBLE);
        else
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        getFromDB();
        addButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onClickSave(true);
            }
        });
    }

    public void onClickSave(boolean show) {
//        if(friendsList==null)
//            friendsList
        for(int i= friendsList.size(); i < 2 ; i++) {
            friendsList.add(new ParseObject("friendDetails"));
        }
        String friend = friendName1.getText().toString();
        String phone = friendPhone1.getText().toString();
        saveToDB(friend, phone, friendsList.get(0));
        Log.i("FriendsPage", friend + " " + phone);
        friend = friendName2.getText().toString();
        phone = friendPhone2.getText().toString();
        saveToDB(friend, phone, friendsList.get(1));
        Log.i("FriendsPage", friend + " " + phone);

        if(show) {
            new AlertDialog.Builder(FriendActivity.this)
                    .setMessage("Friends added/updated successfully..")
                    .setNegativeButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.dismiss();
                            finish();
                        }
                    })
                    .show();
        }
    }

    public void getFromDB() {
        ParseQuery<ParseObject> query = ParseQuery.getQuery("friendDetails");
        query.whereEqualTo("user", ParseUser.getCurrentUser());
        query.findInBackground(new FindCallback<ParseObject>() {
            @Override
            public void done(List<ParseObject> objects, ParseException e) {
                if (e == null) {
                    friendsList = objects;
                    Integer size = objects.size();

                    if( size  == 2) {
                        ParseObject friend1 = objects.get(size - 2);
                        if (friend1 != null) {
                            friendName1.setText(friend1.get("friendName").toString());
                            friendPhone1.setText(friend1.get("friendNumber").toString());
                        }
                    }

                    if(size == 2 || size == 1) {
                        ParseObject friend2 = objects.get(size - 1);
                        if (friend2 != null) {
                            friendName2.setText(friend2.get("friendName").toString());
                            friendPhone2.setText(friend2.get("friendNumber").toString());
                        }
                    }
                }
            }
        });
    }

    public void saveToDB(String friendName, String friendNumber, ParseObject friendDetails) {
        friendDetails.put("user", ParseUser.getCurrentUser());
        friendDetails.put("friendName", friendName);
        friendDetails.put("friendNumber", friendNumber);
        friendDetails.saveInBackground();
    }

    public void onSignup()  {
        onClickSave(false);
        Intent intent = new Intent(this, MainMenu.class);
        startActivity(intent);
        finish();
    }

    public boolean onCreateOptionsMenu(Menu menu) {
        if(isSignUpUser)
            getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                finish();
                return true;

            case R.id.action_done:
                onSignup();
        }

        return super.onOptionsItemSelected(item);
    }

}
