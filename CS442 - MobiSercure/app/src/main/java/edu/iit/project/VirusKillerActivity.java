package edu.iit.project;

import android.app.AlertDialog.Builder;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.view.MenuItem;
import android.view.animation.Animation;
import android.view.animation.RotateAnimation;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import edu.iit.project.utils.MD5Utils;

public class VirusKillerActivity extends AppCompatActivity {

	protected static final int SCANNING = 0;
	protected static final int FINISH = 1;
	private ImageView iv_scan;
	private ProgressBar pb;
	private PackageManager pm;
	private TextView tv_scan_status;
	private LinearLayout ll_scan_content;
	private List<ScanInfo> virusInfos; // list set to store detected virus 
	
	/**
	 * using handle-message mechanism to update UI in Thread(virus detecting)
	 * 
	 * Handler can be used for thread purpose or non-thread purpose
	 */
	private Handler handler = new Handler() {
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case SCANNING:
				ScanInfo scanInfo = (ScanInfo) msg.obj;
				tv_scan_status.setText("scanning: "+scanInfo.appName);
				
				TextView tv = new TextView(getApplicationContext());
				if (scanInfo.isVirus) {
					tv.setTextColor(Color.RED);
					tv.setText("Virus Detected: "+scanInfo.appName);
				} else {
					tv.setTextColor(Color.BLACK);
					tv.setText("Safe File: "+scanInfo.appName);
				}
				ll_scan_content.addView(tv, 0); //every time add at the top(not tail / bottom)
				break;

			case FINISH:
				tv_scan_status.setText("Scan Clear");
				iv_scan.clearAnimation();//stop all animation working on this component
				
				if(virusInfos.size()>0){//Virus found
					Builder builder = new Builder(VirusKillerActivity.this);
					builder.setTitle("Warning!");
					builder.setMessage(virusInfos.size()+" viruses found, We suggest you delete them immediately!");
					builder.setNegativeButton("Cancel", null);
					builder.setPositiveButton("Delete", new OnClickListener() { 
						@Override
						public void onClick(DialogInterface dialog, int which) {
							for(ScanInfo info: virusInfos){
								Intent intent = new Intent(); 
								intent.setAction(Intent.ACTION_DELETE);
								intent.setData(Uri.parse("package:"+info.packageName));
								startActivity(intent);
							}
						}
					});
					builder.show();
				}else{
					Toast.makeText(getApplicationContext(), "Your phone is safe!", Toast.LENGTH_SHORT).show();
				}
				break;
			default:
				break;
			}
		};
	};
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_virus_killer);

		setTitle("Antivirus");
		getSupportActionBar().setDisplayHomeAsUpEnabled(true);
		
		iv_scan = (ImageView) findViewById(R.id.iv_scan);
		pb = (ProgressBar) findViewById(R.id.pb_virus_killer);
		tv_scan_status = (TextView) findViewById(R.id.tv_scan_status);
		ll_scan_content = (LinearLayout) findViewById(R.id.ll_scan_content);
		
		RotateAnimation ra = new RotateAnimation(0, 360, Animation.RELATIVE_TO_SELF, 0.5f, Animation.RELATIVE_TO_SELF, 0.5f);
		ra.setDuration(800);
		ra.setRepeatCount(Animation.INFINITE); 
		
		iv_scan.setAnimation(ra);
		
		scanVirus();
    }
	
	/**
	 * scan virus principle:
	 * obtain file digest, scan the digest with virus database
	 */
	private void scanVirus(){
		tv_scan_status.setText("Booting Virus Killer...");
//		final Message msg = Message.obtain(); //WRONG! one message only serve one time
		virusInfos = new ArrayList<ScanInfo>();
		pm = getPackageManager();
		new Thread(){
			public void run() {
				//pretend take some time to boot up the engine
				try {
					Thread.sleep(500);
				} catch (InterruptedException e1) {
					e1.printStackTrace();
				}
				
				List<PackageInfo> infos = pm.getInstalledPackages(0);
				
				pb.setMax(infos.size());
				int progress = 0;
				
				for (PackageInfo packageInfo : infos) {
					//complete file storage path (data/data/package_name)
					String sourceDir = packageInfo.applicationInfo.sourceDir;
					String md5Digest = MD5Utils.getFileDigest(sourceDir);
					
					ScanInfo scanInfo = new ScanInfo();
					scanInfo.packageName = packageInfo.packageName;
					scanInfo.appName = packageInfo.applicationInfo.loadLabel(pm).toString();
					
					if (VirusKillerDAO.isVirus(md5Digest)) {
						// the file(apk) is virus 
						scanInfo.isVirus = true;
						virusInfos.add(scanInfo);
					}else {
						// the file is not virus
						scanInfo.isVirus = false;
					}
					
					Message msg = Message.obtain();
					msg.obj = scanInfo;
					msg.what = SCANNING;
					handler.sendMessage(msg);
					
					//give users some time to see each scan results 
//					Random random = new Random();
//					try {
//					//	Thread.sleep(40 + random.nextInt(50));
//					} catch (InterruptedException e) {
//						e.printStackTrace();
//					}
					progress ++; //one more file scanned
					pb.setProgress(progress);
				}
				
				Message msg = Message.obtain();
				msg.what = FINISH;
				handler.sendMessage(msg);
			};
		}.start();
		
	}
	
	/*
	 * internal class of scan information
	 */
	class ScanInfo{
		String packageName;
		String appName;
		boolean isVirus;
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
