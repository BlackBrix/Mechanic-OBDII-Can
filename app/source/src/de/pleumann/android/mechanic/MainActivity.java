/*********************************************************************
 * Mechanic - Hacking your car
 *
 * Copyright (C) 2013 Joerg Pleumann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * LICENSE file for more details.
 */
package de.pleumann.android.mechanic;

import java.io.IOException;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

public class MainActivity extends Activity {

    public static final String TAG = "Mechanic";
    
    public static final int DLG_VEHICLE_INFO  = 1000;
    public static final int DLG_TROUBLE_CODES = 1001;
    public static final int DLG_SET_BLUETOOTH = 1002;
    
    private GaugeView speed;
    private GaugeView rpm;
    private GaugeView load;
    private GaugeView temp;
    private GaugeView fuel;

    private volatile boolean alive;

    private volatile String moduleName;

    private volatile BluetoothHelper bt;

    private volatile String vin;
    
    private volatile String[] dtc;
    
    private Runnable bluetoothRunnable = new Runnable() {
        public void run() {
            while (alive) {
                try {
                    doSetTitle(getString(R.string.title_activity_main), getString(R.string.not_connected));
                    boolean showParams = true;
                    
                    if (moduleName != null) {
                        bt = new BluetoothHelper(moduleName);
                        
                        try {
                            while (alive && bt.isConnected()) {
                                try {
                                    String[] vals = bt.receive().split(",");
                                    if (vals[0].equals("vin") && vals.length == 2) {
                                        vin = vals[1];
                                        runOnUiThread(new Runnable() {
                                            public void run() {
                                                showNewDialog(DLG_VEHICLE_INFO);
                                            };
                                        });
                                    } if (vals[0].equals("dtc")) {
                                        dtc = new String[vals.length - 1];
                                        System.arraycopy(vals, 1, dtc, 0, dtc.length);
                                        runOnUiThread(new Runnable() {
                                            public void run() {
                                                showNewDialog(DLG_TROUBLE_CODES);
                                            };
                                        });
                                    } else if (vals[0].equals("mom") && vals.length == 8) {
                                        if (showParams) {
                                            boolean slow = Boolean.parseBoolean(vals[1]);
                                            boolean extended = Boolean.parseBoolean(vals[2]);
                                        
                                            String params = ", "
                                                + (slow ? "250" : "500") + " kbps, "
                                                + getString(extended ? R.string.extended_ids : R.string.standard_ids);
                                            doSetTitle(getString(R.string.title_activity_main), moduleName + params);                 
                                            showParams = false;
                                        }
                                        
                                        speed.setTarget(Float.parseFloat(vals[3]));
                                        rpm.setTarget(Float.parseFloat(vals[4]));
                                        load.setTarget(Float.parseFloat(vals[5]));
                                        temp.setTarget(Float.parseFloat(vals[6]));
                                        fuel.setTarget(Float.parseFloat(vals[7]));
                                    }
                                } catch (Exception e) {
                                    Log.e(TAG, "Error", e);
                                    // Ignored
                                }
                            }
                        } finally {
                            bt.close();
                        }
                    }
                } catch (IOException ioe) {
                    Log.e(TAG, "Error", ioe);
                }
                
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException ie) {
                    // Ignored
                }
            }
            
            bt = null;
        }
    };

    private Runnable animationRunnable = new Runnable() {
        public void run() {
            while (alive) {
                speed.onStep();
                rpm.onStep();
                fuel.onStep();
                load.onStep();
                temp.onStep();
    
                try {
                    Thread.sleep(20);
                } catch (Exception e) {
                    // Ignored
                }
            }
        }
    };
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        speed = (GaugeView)findViewById(R.id.speedGauge);
        speed.setMin(0);
        speed.setMax(255);
        speed.setUnit(" km/h");
        speed.setTarget(0);
        
        rpm = (GaugeView)findViewById(R.id.rpmGauge);
        rpm.setMin(0);
        rpm.setMax(6000);
        rpm.setUnit(getString(R.string.rpm));
        rpm.setTarget(0);

        load = (GaugeView)findViewById(R.id.loadGauge);
        load.setMin(0);
        load.setMax(100);
        load.setUnit(getString(R.string.load));
        load.setTarget(0);

        temp = (GaugeView)findViewById(R.id.tempGauge);
        temp.setMin(-40);
        temp.setMax(215);
        temp.setUnit("¡C");
        temp.setTarget(-40);

        fuel = (GaugeView)findViewById(R.id.fuelGauge);
        fuel.setMin(0);
        fuel.setMax(100);
        fuel.setUnit(getString(R.string.fuel));
        fuel.setTarget(0);
        
        SharedPreferences prefs = getPreferences(MODE_PRIVATE);
        moduleName = prefs.getString("moduleName", null);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    
    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        menu.findItem(R.id.menu_vehicleinfo).setEnabled(bt != null);
        menu.findItem(R.id.menu_troublecodes).setEnabled(bt != null);
        return super.onPrepareOptionsMenu(menu);
    }
    
    @Override
    public boolean onMenuItemSelected(int featureId, MenuItem item) {
        if (item.getItemId() == R.id.menu_vehicleinfo) {
            try {
                bt.send("vin");
            } catch (IOException e) {
                Log.e(TAG, "BT send error", e);
            }
        } else if (item.getItemId() == R.id.menu_troublecodes) {
            try {
                bt.send("dtc");
            } catch (IOException e) {
                Log.e(TAG, "BT send error", e);
            }
        } else if (item.getItemId() == R.id.menu_settings) {
            showNewDialog(DLG_SET_BLUETOOTH);
        } else if (item.getItemId() == R.id.menu_website) {
            Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse("http://code.google.com/p/mechanic/"));
            startActivity(intent);
        }
        return true;
    }

    private void showNewDialog(int id) {
        removeDialog(id);
        showDialog(id);
    }
    
    @Override
    protected Dialog onCreateDialog(int id) {
        if (id == DLG_VEHICLE_INFO) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle(R.string.menu_vehicleinfo);
            builder.setMessage(vin);
            builder.setPositiveButton(R.string.ok, null);
            
            return builder.create();
        } else if (id == DLG_TROUBLE_CODES) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle(R.string.menu_troublecodes);
            builder.setItems(dtc, null);
            builder.setPositiveButton(R.string.ok, null);

            return builder.create();
        } else if (id == DLG_SET_BLUETOOTH) {
            final String[] modules = BluetoothHelper.getBondedDeviceNames();
            
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle(R.string.select_blutooth);
            builder.setItems(modules, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    moduleName = modules[which];
                    SharedPreferences.Editor prefs = getPreferences(MODE_PRIVATE).edit();
                    prefs.putString("moduleName", moduleName);
                    prefs.commit();
                }
            });
            
            return builder.create();
        } else {
            return super.onCreateDialog(id);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        
        alive = true;
        
        new Thread(bluetoothRunnable).start();
        new Thread(animationRunnable).start();
    }
    
    @Override
    protected void onPause() {
        super.onPause();
        
        alive = false;
    }
    
    private void doSetTitle(final String title, final String more) {
        runOnUiThread(new Runnable() {
            public void run() {
                setTitle(title + "Ê[" + more + "]");
            };
        });
        
    }
}
