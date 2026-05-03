# VistarPlanner – Radar Interface Specification

**Document version:** 1.0  
**Date:** April 2026  
**Audience:** External backend integration teams

---

## 1. Overview

VistarPlanner receives real-time data from external backends over **UDP Multicast**.  
All messages are UTF-8 encoded JSON datagrams.

| Parameter | Value |
|-----------|-------|
| Protocol | UDP Multicast |
| Address | `225.0.0.1` |
| Port | `8888` |
| Encoding | UTF-8 JSON |
| Max datagram | 65,535 bytes |

A reference test sender is included at **`test_radar_backend.py`**  
(standard-library only, no pip dependencies).

---

## 2. Common Message Envelope

Every message sent to VistarPlanner **must** include these top-level fields:

| Field | Type | Description |
|-------|------|-------------|
| `STREAM` | string | Message type identifier (see §3) |
| `SRC` | string | Sender identifier – must not be `"MISSION_PLANNER"` |
| `ID` | string | Radar object ID (e.g. `"RADAR_001"`) |

Messages whose `SRC == "MISSION_PLANNER"` are echo-rejected (self-loop guard).

---

## 3. Message Types

### 3.1 `create` – Place / Register a Radar Object

Sent once to place a new radar on the map canvas.  
Optionally includes a `radarAttributes` block to pre-populate the attribute set.

```json
{
    "STREAM":   "create",
    "SRC":      "MY_BACKEND",
    "ID":       "RADAR_001",
    "CLASS":    5,
    "LAT":      28.61,
    "LON":      77.20,
    "ALT":      120.0,
    "HEADING":  0.0,
    "VELOCITY": 0.0,
    "radarAttributes": { ... }
}
```

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `CLASS` | int | yes | Always `5` for RADAR (`eVISTAR_CLASS::VISTAR_CLASS_RADAR`) |
| `LAT` | float | yes | Latitude (decimal degrees, WGS-84) |
| `LON` | float | yes | Longitude (decimal degrees, WGS-84) |
| `ALT` | float | yes | Altitude above MSL (metres) |
| `HEADING` | float | no | Heading in degrees (0–360) |
| `VELOCITY` | float | no | Speed in m/s |
| `radarAttributes` | object | no | Full `RadarAttributes` block (see §4) |

> **Note:** If `radarAttributes` is omitted, VistarPlanner initialises the radar
> with default attribute values which the operator can edit via the GUI.

---

### 3.2 `update` – Update Radar Position / Heading

Used when a radar is mobile.

```json
{
    "STREAM":   "update",
    "SRC":      "MY_BACKEND",
    "ID":       "RADAR_001",
    "CLASS":    5,
    "LAT":      28.615,
    "LON":      77.205,
    "ALT":      120.0,
    "HEADING":  35.0,
    "VELOCITY": 0.0
}
```

Same field schema as `create`.

---

### 3.3 `RadarDetection` – Push Radar Track Data

Sent repeatedly (e.g. once per scan) to update the set of detected tracks.  
The full track list **replaces** the previous one on every message.

```json
{
    "STREAM":    "RadarDetection",
    "SRC":       "MY_BACKEND",
    "ID":        "RADAR_001",
    "TIMESTAMP": "2026-04-14T10:30:00+00:00",
    "DETECTIONS": [
        {
            "trackId":   1,
            "range":     45.2,
            "azimuth":   135.0,
            "elevation": 5.3
        },
        {
            "trackId":   2,
            "range":     82.7,
            "azimuth":   280.0,
            "elevation": 12.1
        }
    ]
}
```

#### Detection Object Fields

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `trackId` | int | – | Unique track number. Consistent across messages. |
| `range` | float | km | Slant range from radar site |
| `azimuth` | float | degrees | 0 = North, increases clockwise (standard radar) |
| `elevation` | float | degrees | Elevation angle above horizon |

Tracks displayed in the **PPI (Plan Position Indicator)** view.  
Hovering over a track marker shows a tooltip with all four values.  
The live track count is synced to `operational.currentTracks` automatically.

---

### 3.4 `RadarAttributes` – Push Live Attribute Updates

Used to push partial or full attribute changes without re-placing the object.  
Typical use cases: EMCON toggle, health degradation, operational mode change.

The `ATTRIBUTES` object may be **partial** (only the changed sub-trees).

```json
{
    "STREAM":     "RadarAttributes",
    "SRC":        "MY_BACKEND",
    "ID":         "RADAR_001",
    "TIMESTAMP":  "2026-04-14T10:35:00+00:00",
    "ATTRIBUTES": {
        "operational": {
            "operationalMode": "Degraded",
            "emconActive":     true,
            "currentTracks":   0
        },
        "maintenance": {
            "healthPct":   65.0,
            "systemHealth": "Degraded"
        }
    }
}
```

> **Merge semantics:** Partial updates are **merged** with the existing stored
> attributes.  Fields not present in the message are left unchanged.

---

### 3.5 `action` + `destroy` – Remove a Radar Object

```json
{
    "STREAM":  "action",
    "SRC":     "MY_BACKEND",
    "ID":      "RADAR_001",
    "ACTION":  "destroy"
}
```

---

## 4. Radar Attributes Schema

Attributes contain a single **`physics`** sub-object (Radar Parameters) that is
displayed in the in-app Radar Parameters editor.

### 4.1 Radar Parameters (Physics)

RF/signal-processing parameters used by the radar equation and simulation backend.

```json
{
    "physics": {
        "Pd":            0.9,
        "Pfa":           1e-6,
        "beamwidth_az":  3.0,
        "beamwidth_el":  4.0,
        "tx_power":      1000.0,
        "pulse_width":   100e-6,
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
```

| Field | Type | Unit | Notes |
|-------|------|------|-------|
| `Pd` | float | – | Probability of detection (0–1) |
| `Pfa` | float | – | Probability of false alarm (e.g. `1e-6`) |
| `beamwidth_az` | float | ° | Azimuth beamwidth |
| `beamwidth_el` | float | ° | Elevation beamwidth |
| `tx_power` | float | W | Transmit power |
| `pulse_width` | float | s | Pulse width (e.g. `100e-6`) |
| `bandwidth` | float | Hz | Signal bandwidth (e.g. `5e6`) |
| `freq_min` | float | Hz | Minimum frequency |
| `freq_max` | float | Hz | Maximum frequency |
| `freq_center` | float | Hz | Centre frequency – auto-computed as `(freq_min + freq_max) / 2` |
| `desired_snr` | float | dB | Required SNR at detection threshold |
| `noise_figure` | float | dB | Receiver noise figure |
| `system_temp` | float | K | System noise temperature |
| `receiver_gain` | float | dB | Receiver gain |
| `prf` | float | Hz | Pulse repetition frequency |
| `scan_time` | float | s | Time for one full scan |
| `rcs` | float | m² | Reference target radar cross-section |
| `loss` | float | dB | Total system loss |

---

## 5. Scenario File Format

Radar attributes are persisted in the scenario JSON under
`additionalData.radarAttributes` for each RADAR object:

```json
{
    "id":        "RADAR_001",
    "type":      "RADAR",
    "latitude":  28.5,
    "longitude": 77.2,
    "altitude":  100.0,
    "additionalData": {
        "range": 150000,
        "radarAttributes": {
            "physics": { ... }
        }
    }
}
```

The `range` field (metres) is the legacy range field used when `radarAttributes` is absent.

---

## 6. PPI View – Attribute Display

The PPI (Plan Position Indicator) widget renders the following attribute-derived
information in real time:

| PPI Element | Source Attribute |
|-------------|-----------------|
| Transmit power | `physics.tx_power` |
| Centre frequency | `physics.freq_center` |
| PRF | `physics.prf` |
| Beamwidth (Az / El) | `physics.beamwidth_az` / `physics.beamwidth_el` |

---

## 7. Operator Attribute Editing

An operator can edit radar parameters through the GUI at any time:

1. **Right-click** a RADAR icon on the map canvas.
2. Select **"Radar Parameters"** from the context menu.
3. The Radar Parameters dialog opens showing all physics / RF parameters.
4. All cells are editable immediately (no separate "edit mode" toggle needed).
5. **Apply** – commits changes without closing the dialog.
6. **OK** – commits changes and closes the dialog.
7. **Cancel** – discards all edits since the last Apply/OK.
8. **Reset to Defaults** – fills all cells with factory defaults.

Changes made via the GUI are:
- Applied immediately to the PPI view.
- Persisted in the next scenario save.
- **Not** sent back to the external backend (one-way: backend → planner).

---

## 8. Quick-start Example (Python)

```python
import socket, json, struct

MCAST = "225.0.0.1"
PORT  = 8888

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, struct.pack('b', 1))

# 1. Place a radar
sock.sendto(json.dumps({
    "STREAM": "create", "SRC": "MY_SYS", "ID": "RADAR_001",
    "CLASS": 5, "LAT": 28.5, "LON": 77.2, "ALT": 100.0,
    "HEADING": 0.0, "VELOCITY": 0.0,
    "radarAttributes": {
        "physics": {"tx_power": 1000.0, "freq_min": 10.0e9, "freq_max": 10.5e9,
                    "Pd": 0.9, "Pfa": 1e-6, "prf": 1000.0, "scan_time": 2.0}
    }
}).encode(), (MCAST, PORT))

# 2. Send detections
sock.sendto(json.dumps({
    "STREAM": "RadarDetection", "SRC": "MY_SYS", "ID": "RADAR_001",
    "TIMESTAMP": "2026-04-14T10:00:00+00:00",
    "DETECTIONS": [
        {"trackId": 1, "range": 45.2, "azimuth": 135.0, "elevation": 5.3},
        {"trackId": 2, "range": 82.7, "azimuth": 280.0, "elevation": 12.1}
    ]
}).encode(), (MCAST, PORT))
```

See **`test_radar_backend.py`** for the full reference implementation including:
- Simulated track trajectories
- Live attribute state updates
- Schema self-verification (`--verify` flag)
- Multicast listen mode (`--listen` flag)

---

## 9. C++ Source Reference

| File | Purpose |
|------|---------|
| `RadarView/radarattributes.h` | `RadarPhysicsParameters`, `RadarAttributes` structs + JSON serialization |
| `RadarView/radarmanager.h/cpp` | `RadarManager` – owns all `Radar` objects + detection lists |
| `RadarView/radarattributesdialog.h/cpp` | Radar Parameters editor dialog |
| `RadarView/radarppiwidget.h/cpp` | PPI display with attribute info panel |
| `RadarView/radardockwidget.h/cpp` | Dockable PPI window per radar |
| `RadarView/radarlistpanel.h/cpp` | Radar list sidebar panel |
| `MapDisplay/cmapcanvas.h/cpp` | Map canvas – parses `RadarDetection` / `RadarAttributes` UDP messages and emits Qt signals |
| `cvistarplanner.h/cpp` | Main window – wires all radar signals to `RadarManager` and opens attribute dialog |
| `sample_scenarios/*.json` | Reference scenario files with full attribute blocks |
| `test_radar_backend.py` | Python test sender / verifier |

---

## 10. Changelog

| Version | Date | Change |
|---------|------|--------|
| 1.0 | April 2026 | Initial release: Radar attribute sub-types, PPI attribute panel, RadarDetection + RadarAttributes UDP streams, operator edit dialog, test backend |
