package edu.iit.project;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.LatLngBounds;
import com.google.android.gms.maps.model.MarkerOptions;
import com.parse.FindCallback;
import com.parse.ParseGeoPoint;
import com.parse.ParseObject;
import com.parse.ParseQuery;
import com.parse.ParseUser;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;

public class MapsActivity extends AppCompatActivity implements OnMapReadyCallback {

    private GoogleMap mMap;
    int selectedDeviceIndex;
    ArrayList<String> devices;
    ArrayList<String> devicesID;
    ArrayList<String> deviceNames;
    Activity activity;

    Button showLocation;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maps);

        activity = this;

        setTitle("Location Tracker");
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        deviceNames = new ArrayList<String>();
        devicesID = new ArrayList<String>();
        // Obtain the SupportMapFragment and get notified when the map is ready to be used.
        devices = (ArrayList<String>) ParseUser.getCurrentUser().get("devices");
        System.out.println("\nDevices: " + devices);
        for(int i=0; i<devices.size(); i++) {
            String deviceList[] = devices.get(i).split(":~-~:");
            if (devices.get(i).equals(getIntent().getStringExtra("SELECTED"))) {
                selectedDeviceIndex = i;
            }
            devicesID.add(deviceList[0]);
            deviceNames.add(deviceList[1]);
        }
        Spinner spinner = (Spinner) findViewById(R.id.deviceSpinner);
        // Create an ArrayAdapter using the string array and a default spinner layout
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, deviceNames);
        // Specify the layout to use when the list of choices appears
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinner.setAdapter(adapter);
        spinner.setSelection(selectedDeviceIndex);
        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                selectedDeviceIndex = position;
                mMap.clear();
                showMapForSelectedDevice();
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        showLocation = (Button) findViewById(R.id.showLocations);

        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager().findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);
    }


    /**
     * Manipulates the map once available.
     * This callback is triggered when the map is ready to be used.
     * This is where we can add markers or lines, add listeners or move the camera. In this case,
     * we just add a marker near Sydney, Australia.
     * If Google Play services is not installed on the device, the user will be prompted to install
     * it inside the SupportMapFragment. This method will only be triggered once the user has
     * installed Google Play services and returned to the app.
     */
    @Override
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;
        showMapForSelectedDevice();
    }


    void showMapForSelectedDevice() {
        ParseQuery<ParseObject> query = ParseQuery.getQuery("location");
        query.whereEqualTo("user", ParseUser.getCurrentUser());
        query.whereEqualTo("device", devicesID.get(selectedDeviceIndex));
        query.addDescendingOrder("createdAt");
        query.findInBackground(new FindCallback<ParseObject>() {
            @Override
            public void done(final List<ParseObject> locationList, com.parse.ParseException e) {
                if (e == null) {
                    LatLngBounds.Builder b = new LatLngBounds.Builder();
                    for (int i = locationList.size()-1; i >= 0; i--) {
                        ParseObject locations = locationList.get(i);
                        ParseGeoPoint geoPoint = (ParseGeoPoint) locations.get("location");
                        LatLng latlng = new LatLng(geoPoint.getLatitude(), geoPoint.getLongitude());
                        b.include(latlng);
                        SimpleDateFormat dt = new SimpleDateFormat("MM-dd-yyyy hh:mm:ss aa");
                        mMap.addMarker(new MarkerOptions().position(latlng)
                                .icon(BitmapDescriptorFactory.defaultMarker(i != 0 ? BitmapDescriptorFactory.HUE_ORANGE : BitmapDescriptorFactory.HUE_BLUE)))
                                .setTitle(dt.format(locations.getCreatedAt()));

                    }
                    if(locationList.size()>0)
                        mMap.moveCamera(CameraUpdateFactory.newLatLngBounds(b.build(), 100));

                    showLocation.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            Intent intent = new Intent(activity, LocationListActivity.class);
                            intent.putExtra("device", devicesID.get(selectedDeviceIndex));
                            intent.putExtra("deviceName", deviceNames.get(selectedDeviceIndex));
                            startActivity(intent);
                        }
                    });


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
}
