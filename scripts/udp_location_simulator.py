#!/usr/bin/env python3
# UDP location simulator for NEV-SmartHMI phone mode.
#
# Pushes fake phone fixes (WGS-84) to the board's PhoneLocationReceiver
# (UDP :45454) at 1 Hz, mimicking what the Android app will send.
#
# Usage:
#   Fixed point (link test):
#     python udp_location_simulator.py --host 10.118.153.187 --duration 60
#   Simulated drive origin -> destination (tests progress + arrival):
#     python udp_location_simulator.py --host 10.118.153.187 --drive --duration 600
#
# Coordinates below are WGS-84 such that after the board's GCJ-02
# conversion they land on the configured fixed origin/destination
# (116.481028,39.989643 -> 116.434446,39.908160).
import argparse
import json
import math
import socket
import time

ORIGIN_WGS = (39.988345, 116.474918)       # lat, lng (WGS-84)
DESTINATION_WGS = (39.906771, 116.428223)  # lat, lng (WGS-84)


def haversine_m(lat1, lng1, lat2, lng2):
    r = 6371000.0
    p = math.pi / 180.0
    dlat = (lat2 - lat1) * p
    dlng = (lng2 - lng1) * p
    a = (math.sin(dlat / 2) ** 2
         + math.cos(lat1 * p) * math.cos(lat2 * p) * math.sin(dlng / 2) ** 2)
    return 2 * r * math.atan2(math.sqrt(a), math.sqrt(1 - a))


def bearing_deg(lat1, lng1, lat2, lng2):
    p = math.pi / 180.0
    dlng = (lng2 - lng1) * p
    y = math.sin(dlng) * math.cos(lat2 * p)
    x = (math.cos(lat1 * p) * math.sin(lat2 * p)
         - math.sin(lat1 * p) * math.cos(lat2 * p) * math.cos(dlng))
    return (math.degrees(math.atan2(y, x)) + 360.0) % 360.0


def main():
    ap = argparse.ArgumentParser(description="Send fake phone GPS fixes over UDP")
    ap.add_argument("--host", required=True, help="board IP, e.g. 10.118.153.187")
    ap.add_argument("--port", type=int, default=45454)
    ap.add_argument("--drive", action="store_true",
                    help="interpolate origin -> destination instead of a fixed point")
    ap.add_argument("--duration", type=float, default=60.0, help="seconds to send")
    ap.add_argument("--accuracy", type=float, default=10.0)
    args = ap.parse_args()

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    total_m = haversine_m(*ORIGIN_WGS, *DESTINATION_WGS)
    steps = max(1, int(args.duration))
    heading = bearing_deg(*ORIGIN_WGS, *DESTINATION_WGS)

    print("target %s:%d, %s, %d packets (route %.1f km)"
          % (args.host, args.port,
             "drive simulation" if args.drive else "fixed point",
             steps, total_m / 1000.0))
    for seq in range(1, steps + 1):
        if args.drive:
            t = (seq - 1) / float(max(1, steps - 1))
            lat = ORIGIN_WGS[0] + (DESTINATION_WGS[0] - ORIGIN_WGS[0]) * t
            lng = ORIGIN_WGS[1] + (DESTINATION_WGS[1] - ORIGIN_WGS[1]) * t
            speed = total_m / max(1.0, args.duration)
        else:
            lat, lng = ORIGIN_WGS
            speed = 0.0
        fix = {
            "version": 1,
            "seq": seq,
            "time": int(time.time() * 1000),
            "lat": round(lat, 6),
            "lng": round(lng, 6),
            "accuracy": args.accuracy,
            "speed": round(speed, 1),
            "bearing": round(heading, 1),
        }
        sock.sendto(json.dumps(fix).encode("ascii"), (args.host, args.port))
        if seq == 1 or seq % 10 == 0:
            print("sent seq=%d lat=%.6f lng=%.6f speed=%.1f"
                  % (seq, lat, lng, speed))
        time.sleep(1.0)
    print("done")


if __name__ == "__main__":
    main()
