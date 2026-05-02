#!/usr/bin/env python3
"""
radar_backend_server.py
=======================
Small UDP-multicast backend that listens for messages from VistarPlanner
(Mission Planner) and pretty-prints every radar *create* message it receives.

The mission planner sends JSON datagrams over UDP multicast whenever a radar
object is placed or updated on the map canvas.  Each create message follows
this protocol::

    {
        "SRC":       "MISSION_PLANNER",
        "ID":        "RADAR_1",
        "CLASS":     "radar",
        "STREAM":    "create",
        "TIMESTAMP": "<ISO-8601 UTC>",
        "LOCATION":  {"X": <lon>, "Y": <lat>, "Z": <alt_m>},
        "ROTATION":  {"PITCH": 0, "ROLL": 0, "YAW": <heading_deg>},
        "parameters": {
            "Pd":            0.9,
            "Pfa":           1e-6,
            "beamwidth_az":  3.0,
            "beamwidth_el":  4.0,
            "tx_power":      1000.0,
            "pulse_width":   1e-4,
            "bandwidth":     5e6,
            "freq_min":      10.0e9,
            "freq_max":      10.5e9,
            "freq_center":   10.25e9,
            "desired_snr":   17.0,
            "noise_figure":  3.0,
            "system_temp":   290.0,
            "receiver_gain": 50.0,
            "prf":           1000.0,
            "scan_time":     2.0,
            "rcs":           2.0,
            "loss":          6.0
        }
    }

Usage
-----
    python3 radar_backend_server.py               # listen only for radar creates
    python3 radar_backend_server.py --all         # print every received message

Dependencies: standard library only (socket, json, struct, datetime)

Network
-------
Protocol : UDP Multicast
Group    : 225.0.0.1
Port     : 8888
"""

import json
import socket
import struct
import sys
from datetime import datetime, timezone

# ── Network constants ─────────────────────────────────────────────────────────
MULTICAST_GRP  = "225.0.0.1"
MULTICAST_PORT = 8888

# ── Source identifier used by VistarPlanner / Mission Planner ─────────────────
VISTAR_SRC_MISSION_PLANNER = "MISSION_PLANNER"


# ── Helper: readable parameter display ────────────────────────────────────────

_PARAM_UNITS = {
    "Pd":            "",
    "Pfa":           "",
    "beamwidth_az":  "°",
    "beamwidth_el":  "°",
    "tx_power":      " W",
    "pulse_width":   " s",
    "bandwidth":     " Hz",
    "freq_min":      " Hz",
    "freq_max":      " Hz",
    "freq_center":   " Hz",
    "desired_snr":   " dB",
    "noise_figure":  " dB",
    "system_temp":   " K",
    "receiver_gain": " dB",
    "prf":           " Hz",
    "scan_time":     " s",
    "rcs":           " m²",
    "loss":          " dB",
}


def _fmt_value(key: str, value) -> str:
    """Format a numeric parameter value with its unit."""
    unit = _PARAM_UNITS.get(key, "")
    if isinstance(value, float):
        # Use scientific notation for very small or very large values
        if abs(value) < 1e-3 or abs(value) >= 1e7:
            return f"{value:.4e}{unit}"
        return f"{value:.6g}{unit}"
    return f"{value}{unit}"


def print_radar_create(msg: dict, src_addr: str) -> None:
    """Pretty-print a radar create message to stdout."""
    sep = "=" * 70
    print(f"\n{sep}")
    print(f"  RADAR CREATE  |  from {src_addr}")
    print(sep)
    print(f"  ID        : {msg.get('ID', '<unknown>')}")
    print(f"  CLASS     : {msg.get('CLASS', '<unknown>')}")
    print(f"  TIMESTAMP : {msg.get('TIMESTAMP', '<none>')}")

    loc = msg.get("LOCATION", {})
    if loc:
        print(f"  LOCATION  : lon={loc.get('X', '?')}  lat={loc.get('Y', '?')}  "
              f"alt={loc.get('Z', '?')} m")

    rot = msg.get("ROTATION", {})
    if rot:
        print(f"  ROTATION  : pitch={rot.get('PITCH', 0)}°  "
              f"roll={rot.get('ROLL', 0)}°  "
              f"yaw={rot.get('YAW', 0)}°")

    params = msg.get("parameters", {})
    if params:
        print(f"\n  {'─' * 50}")
        print("  RADAR PHYSICS PARAMETERS")
        print(f"  {'─' * 50}")
        col_w = max(len(k) for k in params) + 2
        for key, val in params.items():
            print(f"  {key:<{col_w}}: {_fmt_value(key, val)}")
    else:
        print("  (no 'parameters' block in this message)")

    print(sep)


def print_generic(msg: dict, src_addr: str) -> None:
    """Pretty-print any received JSON message."""
    ts = datetime.now(timezone.utc).strftime("%H:%M:%S.%f")[:-3]
    stream  = msg.get("STREAM", "?")
    obj_id  = msg.get("ID", "?")
    src     = msg.get("SRC", "?")
    print(f"[{ts}] [{src_addr}] STREAM={stream:<20}  ID={obj_id:<20}  SRC={src}")


# ── Main listener ─────────────────────────────────────────────────────────────

def run_server(print_all: bool = False) -> None:
    """
    Join the UDP multicast group and print incoming messages.

    :param print_all: If True, print every received message.
                      If False (default), only print radar create messages
                      whose SRC is MISSION_PLANNER.
    """
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    try:
        sock.bind(("", MULTICAST_PORT))
    except OSError as exc:
        print(f"[ERROR] Cannot bind to port {MULTICAST_PORT}: {exc}")
        print("        Is another process already using this port?")
        sys.exit(1)

    group_bin = socket.inet_aton(MULTICAST_GRP)
    mreq      = group_bin + socket.inet_aton("0.0.0.0")
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    print("=" * 70)
    print("  VistarPlanner Radar Backend Server")
    print(f"  Listening on multicast {MULTICAST_GRP}:{MULTICAST_PORT}")
    print(f"  Mode: {'all messages' if print_all else 'radar create messages only'}")
    print("  Press Ctrl-C to stop")
    print("=" * 70)

    try:
        while True:
            data, (src_ip, src_port) = sock.recvfrom(65535)
            src_addr = f"{src_ip}:{src_port}"

            try:
                msg = json.loads(data.decode("utf-8"))
            except (json.JSONDecodeError, UnicodeDecodeError) as exc:
                print(f"[WARN] Non-JSON datagram from {src_addr}: {exc}")
                continue

            stream = msg.get("STREAM", "")
            src    = msg.get("SRC", "")
            cls    = msg.get("CLASS", "")

            if print_all:
                print_generic(msg, src_addr)

            # Focus: radar create messages from the Mission Planner
            if (src == VISTAR_SRC_MISSION_PLANNER
                    and stream == "create"
                    and cls == "radar"):
                print_radar_create(msg, src_addr)

    except KeyboardInterrupt:
        print("\n[INFO] Server stopped by user.")
    finally:
        sock.close()


# ── Entry point ───────────────────────────────────────────────────────────────

if __name__ == "__main__":
    show_all = "--all" in sys.argv
    run_server(print_all=show_all)
