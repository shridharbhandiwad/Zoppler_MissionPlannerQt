#!/usr/bin/env python3
"""
test_radar_backend.py
=====================
Small standalone Python backend that tests the VistarPlanner radar attribute
and detection interface.

Sends four message types over UDP multicast (225.0.0.1:8888):

  1. Object placement message  – places a RADAR object on the map canvas
  2. RadarDetection message     – updates live tracks seen by a radar
  3. RadarAttributes message    – live-updates the radar's attribute set
  4. Object position update     – moves the radar object (optional, same as drone)

Run:
    python3 test_radar_backend.py

Dependencies: standard library only (socket, json, time, math, random)

UDP Multicast: 225.0.0.1:8888   (same channel as VistarPlanner)
"""

import json
import math
import random
import socket
import struct
import time
from datetime import datetime, timezone

# ── Network constants ─────────────────────────────────────────────────────────
MULTICAST_GRP  = "225.0.0.1"
MULTICAST_PORT = 8888
TTL            = 1

# ── Sample radar definitions ──────────────────────────────────────────────────
RADARS = [
    {
        "id": "RADAR_TEST_1",
        "lat": 28.61,
        "lon": 77.20,
        "alt": 120.0,
        "attributes": {
            "design": {
                "radarType":            "Surveillance",
                "manufacturer":         "DRDO",
                "modelDesignation":     "Test-Radar-Alpha",
                "frequencyMHz":         2900.0,
                "bandwidthMHz":         12.0,
                "polarization":         "Linear",
                "maxRangeKm":           150.0,
                "minRangeKm":           1.0,
                "azimuthCovDeg":        360.0,
                "elevationMinDeg":      0.0,
                "elevationMaxDeg":      60.0,
                "peakPowerKw":          250.0,
                "rangeResolutionM":     75.0,
                "azimuthResolutionDeg": 0.4,
                "antennaHeightM":       12.0,
                "weightKg":             3200.0,
                "mountingType":         "Fixed"
            },
            "operational": {
                "operationalMode":   "Active",
                "assignedMission":   "Test Surveillance Run",
                "trackCapacity":     200,
                "currentTracks":     0,
                "emconActive":       False,
                "transmitPowerPct":  100.0,
                "scanRateDegPerSec": 6.0,
                "sectorStartDeg":    0.0,
                "sectorEndDeg":      360.0,
                "iffEnabled":        True,
                "iffMode":           "Mode-3",
                "jammingDetected":   False,
                "jamSignalDbm":      -999.0
            },
            "maintenance": {
                "systemHealth":    "Nominal",
                "healthPct":       98.5,
                "transmitterOk":   True,
                "receiverOk":      True,
                "antennaOk":       True,
                "coolingSysOk":    True,
                "powerSupplyOk":   True,
                "lastServiceDate": "2025-11-01",
                "nextServiceDate": "2026-05-01",
                "operatingHours":  1240,
                "serviceIntervalH":500,
                "mtbfHours":       2000.0,
                "mttrHours":       4.0,
                "maintenanceNotes":"Test backend initial message."
            }
        }
    },
    {
        "id": "RADAR_TEST_2",
        "lat": 28.50,
        "lon": 77.35,
        "alt": 85.0,
        "attributes": {
            "design": {
                "radarType":            "Fire Control",
                "manufacturer":         "BEL",
                "modelDesignation":     "Test-Radar-Beta",
                "frequencyMHz":         9500.0,
                "bandwidthMHz":         50.0,
                "polarization":         "Circular",
                "maxRangeKm":           80.0,
                "minRangeKm":           0.3,
                "azimuthCovDeg":        120.0,
                "elevationMinDeg":      -5.0,
                "elevationMaxDeg":      75.0,
                "peakPowerKw":          120.0,
                "rangeResolutionM":     15.0,
                "azimuthResolutionDeg": 0.2,
                "antennaHeightM":       8.0,
                "weightKg":             1800.0,
                "mountingType":         "Mobile"
            },
            "operational": {
                "operationalMode":   "Active",
                "assignedMission":   "Test Fire Control",
                "trackCapacity":     64,
                "currentTracks":     0,
                "emconActive":       False,
                "transmitPowerPct":  80.0,
                "scanRateDegPerSec": 12.0,
                "sectorStartDeg":    300.0,
                "sectorEndDeg":      60.0,
                "iffEnabled":        True,
                "iffMode":           "Mode-2",
                "jammingDetected":   False,
                "jamSignalDbm":      -999.0
            },
            "maintenance": {
                "systemHealth":    "Nominal",
                "healthPct":       100.0,
                "transmitterOk":   True,
                "receiverOk":      True,
                "antennaOk":       True,
                "coolingSysOk":    True,
                "powerSupplyOk":   True,
                "lastServiceDate": "2025-12-01",
                "nextServiceDate": "2026-06-01",
                "operatingHours":  420,
                "serviceIntervalH":500,
                "mtbfHours":       1800.0,
                "mttrHours":       3.0,
                "maintenanceNotes":""
            }
        }
    }
]

# ── Simulated tracks ──────────────────────────────────────────────────────────
# Each track: (start_range_km, start_az_deg, speed_km_per_tick, heading_deg)
TRACK_TEMPLATES = [
    (30.0,  45.0,  0.8,  90.0),
    (60.0, 135.0,  1.2, 200.0),
    (90.0, 270.0,  0.5,  10.0),
    (45.0, 310.0,  1.0, 150.0),
    (20.0, 180.0,  1.5,  45.0),
]


# ── UDP sender ────────────────────────────────────────────────────────────────
def make_socket():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, struct.pack('b', TTL))
    return sock


def send_json(sock, payload: dict):
    data = json.dumps(payload).encode("utf-8")
    sock.sendto(data, (MULTICAST_GRP, MULTICAST_PORT))
    print(f"  → SENT  STREAM={payload.get('STREAM')}  ID={payload.get('ID')}  "
          f"({len(data)} bytes)")


# ── Message builders ───────────────────────────────────────────────────────────

def build_place_radar(radar: dict) -> dict:
    """
    Object placement message – tells VistarPlanner to create/register a RADAR
    object at the given position and attach its initial attribute set.
    
    Schema:
    {
        "STREAM":     "create",
        "SRC":        "TEST_BACKEND",
        "ID":         "<radar-object-id>",
        "CLASS":      5,            // eVISTAR_CLASS::VISTAR_CLASS_RADAR
        "LAT":        <float>,      // degrees
        "LON":        <float>,      // degrees
        "ALT":        <float>,      // metres
        "HEADING":    0.0,
        "VELOCITY":   0.0,
        "radarAttributes": { ... }  // full RadarAttributes JSON block
    }
    """
    return {
        "STREAM":          "create",
        "SRC":             "TEST_BACKEND",
        "ID":              radar["id"],
        "CLASS":           5,
        "LAT":             radar["lat"],
        "LON":             radar["lon"],
        "ALT":             radar["alt"],
        "HEADING":         0.0,
        "VELOCITY":        0.0,
        "radarAttributes": radar["attributes"]
    }


def build_detection_update(radar_id: str, detections: list) -> dict:
    """
    Radar detection stream message.
    Sent periodically while the radar is scanning.

    Schema:
    {
        "STREAM":     "RadarDetection",
        "SRC":        "TEST_BACKEND",
        "ID":         "<radar-object-id>",
        "TIMESTAMP":  "<ISO8601>",
        "DETECTIONS": [
            {
                "trackId":   <int>,     // unique track number
                "range":     <float>,   // km from radar
                "azimuth":   <float>,   // degrees, 0=North, clockwise
                "elevation": <float>    // degrees above horizontal
            },
            ...
        ]
    }
    """
    return {
        "STREAM":     "RadarDetection",
        "SRC":        "TEST_BACKEND",
        "ID":         radar_id,
        "TIMESTAMP":  datetime.now(timezone.utc).isoformat(),
        "DETECTIONS": detections
    }


def build_attrs_update(radar_id: str, attrs: dict) -> dict:
    """
    Live attribute update message.
    Used to push operational / maintenance state changes in real-time
    (e.g. EMCON activated, track count changed, health degraded).

    Schema:
    {
        "STREAM":     "RadarAttributes",
        "SRC":        "TEST_BACKEND",
        "ID":         "<radar-object-id>",
        "TIMESTAMP":  "<ISO8601>",
        "ATTRIBUTES": { ... }   // partial or full RadarAttributes JSON block
    }
    """
    return {
        "STREAM":     "RadarAttributes",
        "SRC":        "TEST_BACKEND",
        "ID":         radar_id,
        "TIMESTAMP":  datetime.now(timezone.utc).isoformat(),
        "ATTRIBUTES": attrs
    }


# ── Track simulation ──────────────────────────────────────────────────────────

class TrackSimulator:
    def __init__(self, track_id: int, start_range: float, start_az: float,
                 speed: float, heading: float, max_range: float):
        self.track_id   = track_id
        self.range_km   = start_range
        self.azimuth    = start_az
        self.speed      = speed       # km per tick
        self.heading    = heading     # degrees (0=N,90=E,...)
        self.elevation  = random.uniform(1.0, 15.0)
        self.max_range  = max_range

    def tick(self):
        # Move the track in the direction of heading
        dx = self.speed * math.sin(math.radians(self.heading))
        dy = self.speed * math.cos(math.radians(self.heading))
        # Convert to polar increments (approximate)
        self.range_km = max(0.5, self.range_km + dy)
        self.azimuth  = (self.azimuth + dx / max(self.range_km, 1.0) *
                         math.degrees(1)) % 360.0
        self.elevation += random.uniform(-0.5, 0.5)
        self.elevation  = max(0.5, min(self.elevation, 30.0))

    def to_detection(self) -> dict:
        return {
            "trackId":   self.track_id,
            "range":     round(self.range_km, 2),
            "azimuth":   round(self.azimuth, 1),
            "elevation": round(self.elevation, 1)
        }

    @property
    def visible(self) -> bool:
        return self.range_km < self.max_range


# ── Main test sequence ────────────────────────────────────────────────────────

def run_test():
    print("=" * 60)
    print("  VistarPlanner Radar Test Backend")
    print("  Multicast: %s:%d" % (MULTICAST_GRP, MULTICAST_PORT))
    print("=" * 60)

    sock = make_socket()

    # Phase 1: Place radars on map
    print("\n[Phase 1] Sending object placement messages ...")
    for radar in RADARS:
        send_json(sock, build_place_radar(radar))
        time.sleep(0.3)

    # Phase 2: Send initial attribute blobs separately (covers RadarAttributes stream)
    print("\n[Phase 2] Sending initial RadarAttributes messages ...")
    for radar in RADARS:
        send_json(sock, build_attrs_update(radar["id"], radar["attributes"]))
        time.sleep(0.2)

    # Phase 3: Run detection simulation for 60 ticks
    print("\n[Phase 3] Starting detection simulation (60 ticks, 1s each) ...")
    max_range_1 = RADARS[0]["attributes"]["design"]["maxRangeKm"]
    max_range_2 = RADARS[1]["attributes"]["design"]["maxRangeKm"]

    tracks_1 = [
        TrackSimulator(tid + 1, rng, az, spd, hdg, max_range_1)
        for tid, (rng, az, spd, hdg) in enumerate(TRACK_TEMPLATES[:3])
    ]
    tracks_2 = [
        TrackSimulator(tid + 101, rng, az, spd, hdg, max_range_2)
        for tid, (rng, az, spd, hdg) in enumerate(TRACK_TEMPLATES[3:])
    ]

    for tick in range(60):
        print(f"\n  -- Tick {tick + 1:02d} --")

        # Advance tracks
        for t in tracks_1 + tracks_2:
            t.tick()

        # Build detection lists for each radar
        dets_1 = [t.to_detection() for t in tracks_1 if t.visible]
        dets_2 = [t.to_detection() for t in tracks_2 if t.visible]

        send_json(sock, build_detection_update(RADARS[0]["id"], dets_1))
        send_json(sock, build_detection_update(RADARS[1]["id"], dets_2))

        # Every 10 ticks send a live attribute update to simulate state change
        if (tick + 1) % 10 == 0:
            # Simulate EMCON toggle on radar 2 at tick 20, off at tick 40
            emcon = (tick + 1) == 20
            health = max(70.0, 100.0 - tick * 0.3)

            attrs_update_1 = {
                "operational": {
                    "currentTracks": len(dets_1),
                    "jammingDetected": random.random() < 0.05,
                },
                "maintenance": {
                    "healthPct": round(health, 1),
                    "systemHealth": "Nominal" if health >= 80 else "Degraded"
                }
            }
            attrs_update_2 = {
                "operational": {
                    "currentTracks": len(dets_2),
                    "emconActive":   emcon,
                    "operationalMode": "Standby" if emcon else "Active",
                }
            }

            send_json(sock, build_attrs_update(RADARS[0]["id"], attrs_update_1))
            send_json(sock, build_attrs_update(RADARS[1]["id"], attrs_update_2))

        time.sleep(1.0)

    print("\n[Done] Test sequence complete.")
    sock.close()


# ── Verification helper ───────────────────────────────────────────────────────

def verify_attributes_schema():
    """
    Offline JSON schema self-verification – prints structured check of each
    attribute subtype to confirm correct field names and types.
    No network connection required.
    """
    print("\n=== Attribute Schema Verification ===\n")

    for radar in RADARS:
        attrs = radar["attributes"]
        radar_id = radar["id"]
        print(f"Radar: {radar_id}")

        design = attrs["design"]
        assert isinstance(design["frequencyMHz"], float), "frequencyMHz must be float"
        assert isinstance(design["maxRangeKm"], float),   "maxRangeKm must be float"
        assert design["azimuthCovDeg"] > 0,               "azimuthCovDeg must be positive"
        print("  [PASS] design attributes")

        op = attrs["operational"]
        assert op["operationalMode"] in (
            "Standby", "Active", "Maintenance", "Degraded", "Off"
        ), f"Invalid operationalMode: {op['operationalMode']}"
        assert 0.0 <= op["transmitPowerPct"] <= 100.0, "transmitPowerPct out of range"
        assert isinstance(op["iffEnabled"], bool),          "iffEnabled must be bool"
        print("  [PASS] operational attributes")

        mnt = attrs["maintenance"]
        assert 0.0 <= mnt["healthPct"] <= 100.0, "healthPct out of range"
        assert mnt["systemHealth"] in (
            "Nominal", "Degraded", "Critical", "Failed"
        ), f"Invalid systemHealth: {mnt['systemHealth']}"
        print("  [PASS] maintenance attributes\n")

    print("=== All schema checks passed ===\n")


# ── Listen mode – print all received UDP messages ─────────────────────────────

def listen_mode():
    """
    Join the multicast group and print every received JSON message.
    Useful to monitor what VistarPlanner is broadcasting.
    """
    print("Listening on %s:%d  (Ctrl-C to stop)" % (MULTICAST_GRP, MULTICAST_PORT))

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(("", MULTICAST_PORT))

    group = socket.inet_aton(MULTICAST_GRP)
    mreq  = group + socket.inet_aton("0.0.0.0")
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    try:
        while True:
            data, addr = sock.recvfrom(65535)
            try:
                msg = json.loads(data.decode("utf-8"))
                print(f"[{addr[0]}] {json.dumps(msg, indent=2)}\n")
            except json.JSONDecodeError:
                print(f"[{addr[0]}] (raw) {data[:200]!r}\n")
    except KeyboardInterrupt:
        print("\nListener stopped.")
    finally:
        sock.close()


# ── Entry point ───────────────────────────────────────────────────────────────

if __name__ == "__main__":
    import sys

    if "--verify" in sys.argv:
        verify_attributes_schema()
    elif "--listen" in sys.argv:
        listen_mode()
    else:
        verify_attributes_schema()   # run check first
        run_test()
