package com.nev.locator;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.nio.charset.StandardCharsets;

/**
 * Minimal fire-and-forget UDP sender. Each packet is a single UTF-8 JSON
 * datagram sent to boardIp:45454, matching the board-side
 * PhoneLocationReceiver protocol (version 1).
 */
public final class UdpSender {
    public static final int BOARD_PORT = 45454;

    private UdpSender() {}

    /** Sends one datagram on a background thread; failures are swallowed. */
    public static void sendAsync(final String boardIp, final String json) {
        new Thread(new Runnable() {
            @Override public void run() {
                DatagramSocket socket = null;
                try {
                    byte[] payload = json.getBytes(StandardCharsets.UTF_8);
                    InetAddress addr = InetAddress.getByName(boardIp);
                    DatagramPacket packet = new DatagramPacket(payload, payload.length, addr, BOARD_PORT);
                    socket = new DatagramSocket();
                    socket.send(packet);
                } catch (Exception ignored) {
                    // UDP best effort; board side counts rejects/timeouts
                } finally {
                    if (socket != null) socket.close();
                }
            }
        }).start();
    }

    /** Builds a location fix datagram (WGS-84; board converts to GCJ-02). */
    public static String buildLocationJson(long seq, long timeMs, double lat, double lng,
                                           float accuracy, float speedMps, float bearingDeg) {
        return "{\"version\":1,\"seq\":" + seq
                + ",\"time\":" + timeMs
                + ",\"lat\":" + lat
                + ",\"lng\":" + lng
                + ",\"accuracy\":" + accuracy
                + ",\"speed\":" + speedMps
                + ",\"bearing\":" + bearingDeg + "}";
    }

    /** Builds a destination datagram (GCJ-02; board uses it as-is). */
    public static String buildDestinationJson(String name, double lat, double lng) {
        StringBuilder sb = new StringBuilder("{\"version\":1,\"type\":\"destination\"");
        if (name != null && !name.isEmpty()) {
            sb.append(",\"name\":\"").append(escape(name)).append("\"");
        }
        sb.append(",\"lat\":").append(lat).append(",\"lng\":").append(lng).append("}");
        return sb.toString();
    }

    private static String escape(String s) {
        StringBuilder out = new StringBuilder(s.length() + 8);
        for (int i = 0; i < s.length(); i++) {
            char c = s.charAt(i);
            switch (c) {
                case '"': out.append("\\\""); break;
                case '\\': out.append("\\\\"); break;
                case '\n': out.append("\\n"); break;
                case '\r': out.append("\\r"); break;
                case '\t': out.append("\\t"); break;
                default:
                    if (c < 0x20) {
                        out.append(String.format("\\u%04x", (int) c));
                    } else {
                        out.append(c);
                    }
            }
        }
        return out.toString();
    }
}
