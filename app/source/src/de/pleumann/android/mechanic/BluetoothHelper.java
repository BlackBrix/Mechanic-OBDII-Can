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
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

public class BluetoothHelper {

    public static final String TAG = "Mechanic";
    
    private BluetoothDevice device;

    private BluetoothSocket socket;

    private InputStream input;

    private OutputStream output;

    public BluetoothHelper(String name) throws IOException {
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        Set<BluetoothDevice> devices = adapter.getBondedDevices();
        for (BluetoothDevice d: devices) {
            if (d.getName().equals(name)) {
                device = d;
                open();
                return;
            }
        }
        
        throw new IOException("Bluetooth device " + name + " not found");
    }

    public static final String[] getBondedDeviceNames() {
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        Set<BluetoothDevice> devices = adapter.getBondedDevices();
        String[] names = new String[devices.size()];
        
        int i = 0;
        for (BluetoothDevice d: devices) {
            Log.d(TAG, "Found bonded device " + d.getName());
            names[i++] = d.getName();
        }
        
        return names;
    }
    
    private void open() throws IOException {
        Log.d(TAG, "Opening connection to " + device.getName());
        
        try {
            socket = device.createInsecureRfcommSocketToServiceRecord(UUID
                    .fromString("00001101-0000-1000-8000-00805F9B34FB"));
            socket.connect();
    
            input = socket.getInputStream();
            output = socket.getOutputStream();
        } catch (IOException e) {
            close();
            throw e;
        }
        
        Log.d(TAG, "Connected");
    }
    
    public boolean isConnected() {
        return socket != null && socket.isConnected();
    }
    
    public void send(String s) throws IOException {
        output.write(s.getBytes());
        output.write(13);
        output.write(10);
        output.flush();
    }

    public String receive() throws IOException {
        StringBuilder builder = new StringBuilder(256);
        int i = input.read();
        while (i != 13) {
            if (i >= ' ') {
                builder.append((char)i);
            }
            i = input.read();
        }

        return builder.toString();
    }

    public void close() {
        Log.d(TAG, "Closing connection to " + device.getName());
        
        try {
            if (input != null) {
                input.close();
                input = null;
            }
        } catch (Exception ignored) {
        }

        try {
            if (output != null) {
                output.close();
                output = null;
            }
        } catch (Exception ignored) {
        }

        try {
            if (socket != null) {
                socket.close();
                socket = null;
            }
        } catch (Exception ignored) {
        }

        if (device != null) {
            device = null;
        }

        Log.d(TAG, "Disconnected");
    }

}
