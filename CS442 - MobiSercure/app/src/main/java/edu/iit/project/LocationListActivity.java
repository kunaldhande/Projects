package edu.iit.project;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

import com.parse.FindCallback;
import com.parse.ParseObject;
import com.parse.ParseQuery;
import com.parse.ParseUser;

import java.text.SimpleDateFormat;
import java.util.List;



public class LocationListActivity extends AppCompatActivity {
    Activity activity;
    ListView listview;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_location_list);

        activity = this;

        listview = (ListView) findViewById(R.id.listView);
        String device = getIntent().getStringExtra("device");

        setTitle("Recent Locations: " + getIntent().getStringExtra("deviceName"));
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        ParseQuery<ParseObject> query = ParseQuery.getQuery("location");
        query.whereEqualTo("user", ParseUser.getCurrentUser());
        query.whereEqualTo("device", device);
        query.addDescendingOrder("createdAt");
        query.setLimit(20);
        query.findInBackground(new FindCallback<ParseObject>() {
            @Override
            public void done(List<ParseObject> locationList, com.parse.ParseException e) {
                if (e == null) {
                    LocationAdapter locationAdapter = new LocationAdapter(activity, android.R.layout.simple_list_item_1, locationList);
                    listview.setAdapter(locationAdapter);
                } else {
                    Log.d("Location", "Error: " + e.getMessage());
                }
            }
        });
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



    class LocationAdapter extends ArrayAdapter<ParseObject> {
        List<ParseObject> result;
        private LayoutInflater inflater=null;

        public LocationAdapter(Context context, int resource, List<ParseObject> locationList) {
            // TODO Auto-generated constructor stub
            super(context, resource);
            result=locationList;
            inflater = (LayoutInflater) context.
                    getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }
        @Override
        public int getCount() {
            // TODO Auto-generated method stub
            return result.size();
        }

        @Override
        public long getItemId(int position) {
            // TODO Auto-generated method stub
            return position;
        }

        public class Holder
        {
            TextView timeStamp;
            TextView address;
        }
        @Override
        public View getView(final int position, final View convertView, ViewGroup parent) {
            // TODO Auto-generated method stub
            final Holder holder=new Holder();
            final View rowView;
            final ParseObject item = result.get(position);

            rowView = inflater.inflate(R.layout.location_list, null);

            holder.timeStamp=(TextView) rowView.findViewById(R.id.time);
            holder.address=(TextView) rowView.findViewById(R.id.address);

            SimpleDateFormat dt = new SimpleDateFormat("MM-dd-yyyy hh:mm:ss aa");
            holder.timeStamp.setText(dt.format(item.getCreatedAt()));
            if(item.get("addresss")!=null)
                holder.address.setText(item.get("addresss").toString());
            else
                holder.address.setText("");

            return rowView;
        }
    }
}
