package com.nev.locator;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ServiceInfo;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;

/**
 * Foreground service: takes GPS fixes from LocationManager and forwards one
 * JSON datagram per fix (throttled to >= 1 s) to the board over UDP 45454.
 *
 * Coordinates are sent as WGS-84 (Android GPS native); the board converts
 * to GCJ-02. seq is strictly increasing per service run, as required by the
 * board-side validator.
 */
public class LocationService extends Service implements LocationListener {

    public static final String ACTION_START = "com.nev.locator.START";
    public static final String ACTION_STOP = "com.nev.locator.STOP";
    public static final String BROADCAST_FIX = "com.nev.locator.FIX";
    public static final String EXTRA_TEXT = "text";

    private static final String CHANNEL_ID = "locator";
    private static final int NOTIFICATION_ID = 1;
    private static final long MIN_INTERVAL_MS = 1000;

    private LocationManager locationManager;
    private long seq = 0;
    private long lastSentMs = 0;
    private String boardIp = "10.118.153.187";

    @Override
    public void onCreate() {
        super.onCreate();
        locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
        SharedPreferences prefs = getSharedPreferences("nev", MODE_PRIVATE);
        boardIp = prefs.getString("board_ip", boardIp);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (intent != null && ACTION_STOP.equals(intent.getAction())) {
            stopSelf();
            return START_NOT_STICKY;
        }
        SharedPreferences prefs = getSharedPreferences("nev", MODE_PRIVATE);
        boardIp = prefs.getString("board_ip", boardIp);
        startForegroundWithNotification();
        startLocationUpdates();
        return START_STICKY;
    }

    private void startForegroundWithNotification() {
        NotificationManager nm = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        if (Build.VERSION.SDK_INT >= 26) {
            NotificationChannel channel = new NotificationChannel(
                    CHANNEL_ID, getString(R.string.channel_name), NotificationManager.IMPORTANCE_LOW);
            nm.createNotificationChannel(channel);
        }
        Intent openIntent = new Intent(this, MainActivity.class);
        PendingIntent pi = PendingIntent.getActivity(this, 0, openIntent,
                PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);
        Notification.Builder builder = Build.VERSION.SDK_INT >= 26
                ? new Notification.Builder(this, CHANNEL_ID)
                : new Notification.Builder(this);
        Notification notification = builder
                .setContentTitle("NEV-Locator 正在推送定位")
                .setContentText("目标 " + boardIp + ":" + UdpSender.BOARD_PORT)
                .setSmallIcon(android.R.drawable.ic_menu_mylocation)
                .setContentIntent(pi)
                .setOngoing(true)
                .build();
        if (Build.VERSION.SDK_INT >= 29) {
            startForeground(NOTIFICATION_ID, notification,
                    ServiceInfo.FOREGROUND_SERVICE_TYPE_LOCATION);
        } else {
            startForeground(NOTIFICATION_ID, notification);
        }
    }

    private void startLocationUpdates() {
        try {
            locationManager.requestLocationUpdates(
                    LocationManager.GPS_PROVIDER, 500, 0, this);
            // Network provider as fallback indoors; still WGS-84-ish, marked
            // by its accuracy and gated board-side (accuracy <= 50 m).
            locationManager.requestLocationUpdates(
                    LocationManager.NETWORK_PROVIDER, 500, 0, this);
        } catch (SecurityException e) {
            broadcast("权限不足，无法定位");
            stopSelf();
        }
    }

    @Override
    public void onLocationChanged(Location location) {
        long now = System.currentTimeMillis();
        if (now - lastSentMs < MIN_INTERVAL_MS) return;
        lastSentMs = now;
        seq++;

        float accuracy = location.hasAccuracy() ? location.getAccuracy() : 9999f;
        float speed = location.hasSpeed() ? location.getSpeed() : 0f;
        float bearing = location.hasBearing() ? location.getBearing() : 0f;

        String json = UdpSender.buildLocationJson(
                seq, now, location.getLatitude(), location.getLongitude(),
                accuracy, speed, bearing);
        UdpSender.sendAsync(boardIp, json);

        broadcast(String.format("#%d  %.6f, %.6f  ±%.0fm  %.1fkm/h",
                seq, location.getLatitude(), location.getLongitude(), accuracy, speed * 3.6f));
    }

    private void broadcast(String text) {
        Intent intent = new Intent(BROADCAST_FIX);
        intent.setPackage(getPackageName());
        intent.putExtra(EXTRA_TEXT, text);
        sendBroadcast(intent);
    }

    @Override
    public void onDestroy() {
        if (locationManager != null) locationManager.removeUpdates(this);
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onProviderEnabled(String provider) {}

    @Override
    public void onProviderDisabled(String provider) {}

    @Override
    @Deprecated
    public void onStatusChanged(String provider, int status, Bundle extras) {}
}
