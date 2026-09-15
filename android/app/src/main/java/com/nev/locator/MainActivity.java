package com.nev.locator;

import android.Manifest;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.graphics.drawable.GradientDrawable;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;

/**
 * NEV-Locator main screen.
 *
 *  - Board IP + AMap web-service key are persisted in SharedPreferences.
 *  - "开始推送" runs the foreground LocationService (1 Hz GPS -> UDP 45454).
 *  - "发送目的地" geocodes a Chinese place name via the AMap web API
 *    (returns GCJ-02) and sends a type=destination datagram to the board.
 */
public class MainActivity extends Activity {

    private EditText ipEdit;
    private EditText keyEdit;
    private EditText destinationEdit;
    private TextView statusText;
    private View statusDot;
    private Button startButton;
    private Button stopButton;

    /** 目的地操作后 4 秒内不被每秒的定位状态刷新覆盖 */
    private long destStatusHoldUntilMs = 0;

    /** 状态指示灯：绿=收到定位，青=推送中，红=失败，灰=未启动/已停止 */
    private void setStatusDot(int colorRes) {
        if (statusDot == null) return;
        GradientDrawable dot = (GradientDrawable) statusDot.getBackground().mutate();
        dot.setColor(getResources().getColor(colorRes));
    }

    private final BroadcastReceiver fixReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (System.currentTimeMillis() < destStatusHoldUntilMs) return;
            String text = intent.getStringExtra(LocationService.EXTRA_TEXT);
            if (text != null) {
                statusText.setText(text);
                setStatusDot(R.color.nev_green);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ipEdit = findViewById(R.id.editIp);
        keyEdit = findViewById(R.id.editKey);
        destinationEdit = findViewById(R.id.editDestination);
        statusText = findViewById(R.id.textStatus);
        statusDot = findViewById(R.id.statusDot);
        startButton = findViewById(R.id.buttonStart);
        stopButton = findViewById(R.id.buttonStop);
        Button sendDestinationButton = findViewById(R.id.buttonSendDestination);

        SharedPreferences prefs = getSharedPreferences("nev", MODE_PRIVATE);
        ipEdit.setText(prefs.getString("board_ip", "10.118.153.187"));
        keyEdit.setText(prefs.getString("amap_key", ""));

        startButton.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View v) {
                savePrefs();
                if (ensurePermissions()) startServiceNow();
            }
        });
        stopButton.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, LocationService.class);
                intent.setAction(LocationService.ACTION_STOP);
                startService(intent);
                statusText.setText("已停止");
                setStatusDot(R.color.nev_gray);
            }
        });
        sendDestinationButton.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View v) {
                savePrefs();
                sendDestination();
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        IntentFilter filter = new IntentFilter(LocationService.BROADCAST_FIX);
        if (Build.VERSION.SDK_INT >= 33) {
            registerReceiver(fixReceiver, filter, Context.RECEIVER_NOT_EXPORTED);
        } else {
            registerReceiver(fixReceiver, filter);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(fixReceiver);
    }

    private void savePrefs() {
        getSharedPreferences("nev", MODE_PRIVATE).edit()
                .putString("board_ip", ipEdit.getText().toString().trim())
                .putString("amap_key", keyEdit.getText().toString().trim())
                .apply();
    }

    private boolean ensurePermissions() {
        if (Build.VERSION.SDK_INT < 23) return true;
        java.util.ArrayList<String> wanted = new java.util.ArrayList<>();
        if (checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION)
                != PackageManager.PERMISSION_GRANTED) {
            wanted.add(Manifest.permission.ACCESS_FINE_LOCATION);
        }
        if (Build.VERSION.SDK_INT >= 33
                && checkSelfPermission(Manifest.permission.POST_NOTIFICATIONS)
                != PackageManager.PERMISSION_GRANTED) {
            wanted.add(Manifest.permission.POST_NOTIFICATIONS);
        }
        if (wanted.isEmpty()) return true;
        requestPermissions(wanted.toArray(new String[0]), 1);
        return false;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 1 && ensurePermissions()) startServiceNow();
    }

    private void startServiceNow() {
        Intent intent = new Intent(this, LocationService.class);
        intent.setAction(LocationService.ACTION_START);
        if (Build.VERSION.SDK_INT >= 26) {
            startForegroundService(intent);
        } else {
            startService(intent);
        }
        statusText.setText("推送中 -> " + ipEdit.getText().toString().trim()
                + ":" + UdpSender.BOARD_PORT);
        setStatusDot(R.color.nev_accent);
    }

    /** Geocodes the typed Chinese name (AMap -> GCJ-02) and sends it. */
    private void sendDestination() {
        final String name = destinationEdit.getText().toString().trim();
        final String key = keyEdit.getText().toString().trim();
        final String boardIp = ipEdit.getText().toString().trim();
        if (name.isEmpty()) {
            toast("请输入目的地名称");
            return;
        }
        if (key.isEmpty()) {
            toast("请先填高德 Web 服务 Key");
            return;
        }
        statusText.setText("地理编码中: " + name);
        destStatusHoldUntilMs = System.currentTimeMillis() + 4000;
        new Thread(new Runnable() {
            @Override public void run() {
                try {
                    String url = "https://restapi.amap.com/v3/geocode/geo?key="
                            + URLEncoder.encode(key, "UTF-8")
                            + "&address=" + URLEncoder.encode(name, "UTF-8");
                    HttpURLConnection conn = (HttpURLConnection) new URL(url).openConnection();
                    conn.setConnectTimeout(8000);
                    conn.setReadTimeout(8000);
                    BufferedReader reader = new BufferedReader(
                            new InputStreamReader(conn.getInputStream(), "UTF-8"));
                    StringBuilder body = new StringBuilder();
                    String line;
                    while ((line = reader.readLine()) != null) body.append(line);
                    reader.close();
                    conn.disconnect();

                    JSONObject root = new JSONObject(body.toString());
                    if (!"1".equals(root.optString("status"))) {
                        failOnUi("地理编码失败: " + root.optString("info"));
                        return;
                    }
                    JSONArray geocodes = root.optJSONArray("geocodes");
                    if (geocodes == null || geocodes.length() == 0) {
                        failOnUi("找不到该地点");
                        return;
                    }
                    JSONObject first = geocodes.getJSONObject(0);
                    String[] parts = first.getString("location").split(",");
                    final double lng = Double.parseDouble(parts[0]);
                    final double lat = Double.parseDouble(parts[1]);
                    final String formatted = first.optString("formatted_address", name);

                    UdpSender.sendAsync(boardIp, UdpSender.buildDestinationJson(formatted, lat, lng));
                    runOnUiThread(new Runnable() {
                        @Override public void run() {
                            statusText.setText(String.format(
                                    "目的地已发送: %s\n%.6f, %.6f", formatted, lat, lng));
                            destStatusHoldUntilMs = System.currentTimeMillis() + 4000;
                        }
                    });
                } catch (Exception e) {
                    failOnUi("发送失败: " + e.getMessage());
                }
            }
        }).start();
    }

    private void failOnUi(final String message) {
        runOnUiThread(new Runnable() {
            @Override public void run() {
                statusText.setText(message);
                setStatusDot(R.color.nev_red);
                destStatusHoldUntilMs = System.currentTimeMillis() + 4000;
                toast(message);
            }
        });
    }

    private void toast(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }
}
