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

Attributes are grouped into three **sub-categories** that map directly to the
three tabs shown in the in-app Attributes editor.

### 4.1 Design Attributes

Static / hardware parameters that rarely change in the field.

```json
{
    "design": {
        "radarType":            "Surveillance",
        "manufacturer":         "DRDO",
        "modelDesignation":     "Arudhra-1",
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
    }
}
```

| Field | Type | Unit | Allowed Values / Notes |
|-------|------|------|------------------------|
| `radarType` | string | – | `Surveillance` \| `Fire Control` \| `Weather` \| `SAR` \| `Tracking` |
| `manufacturer` | string | – | Free text |
| `modelDesignation` | string | – | Free text |
| `frequencyMHz` | float | MHz | Operating centre frequency |
| `bandwidthMHz` | float | MHz | Instantaneous bandwidth |
| `polarization` | string | – | `Linear` \| `Circular` \| `Dual` |
| `maxRangeKm` | float | km | Maximum instrumented range; drives PPI ring labels |
| `minRangeKm` | float | km | Minimum detection range (blind zone) |
| `azimuthCovDeg` | float | ° | 360 = full omnidirectional coverage |
| `elevationMinDeg` | float | ° | Lower elevation limit |
| `elevationMaxDeg` | float | ° | Upper elevation limit |
| `peakPowerKw` | float | kW | Transmitter peak power |
| `rangeResolutionM` | float | m | Minimum resolvable range difference |
| `azimuthResolutionDeg` | float | ° | Angular resolution |
| `antennaHeightM` | float | m | Height above ground |
| `weightKg` | float | kg | System weight |
| `mountingType` | string | – | `Fixed` \| `Mobile` \| `Ship-borne` \| `Airborne` |

**Default values** (used when not supplied by backend):

| Field | Default |
|-------|---------|
| `radarType` | `"Surveillance"` |
| `frequencyMHz` | `3000.0` |
| `polarization` | `"Linear"` |
| `maxRangeKm` | `150.0` |
| `azimuthCovDeg` | `360.0` |
| `peakPowerKw` | `100.0` |
| `mountingType` | `"Fixed"` |
| All others | See `radarattributes.h` |

---

### 4.2 Operational Attributes

Mission-time / live parameters that change during operation.

```json
{
    "operational": {
        "operationalMode":   "Active",
        "assignedMission":   "Area Surveillance",
        "trackCapacity":     200,
        "currentTracks":     3,
        "emconActive":       false,
        "transmitPowerPct":  100.0,
        "scanRateDegPerSec": 6.0,
        "sectorStartDeg":    0.0,
        "sectorEndDeg":      360.0,
        "iffEnabled":        true,
        "iffMode":           "Mode-3",
        "jammingDetected":   false,
        "jamSignalDbm":      -999.0
    }
}
```

| Field | Type | Unit | Allowed Values / Notes |
|-------|------|------|------------------------|
| `operationalMode` | string | – | `Standby` \| `Active` \| `Maintenance` \| `Degraded` \| `Off` |
| `assignedMission` | string | – | Free text |
| `trackCapacity` | int | tracks | Max simultaneous tracks |
| `currentTracks` | int | tracks | Live count; auto-updated by RadarDetection messages |
| `emconActive` | bool | – | Emission control (silent mode); shown as badge in PPI |
| `transmitPowerPct` | float | % | 0–100 |
| `scanRateDegPerSec` | float | °/s | Antenna rotation speed |
| `sectorStartDeg` | float | ° | Start of active coverage sector (0=North) |
| `sectorEndDeg` | float | ° | End of active coverage sector |
| `iffEnabled` | bool | – | IFF interrogator active |
| `iffMode` | string | – | `Mode-1` \| `Mode-2` \| `Mode-3` \| `Mode-S` |
| `jammingDetected` | bool | – | Shown as `JAM` badge in PPI when true |
| `jamSignalDbm` | float | dBm | Jamming signal power; `-999` = none detected |

**PPI visual indicators:**
- `operationalMode` drives the mode pill colour (green=Active, yellow=Standby, orange=Degraded, red=Off)
- `emconActive = true` → `EMCON` badge
- `jammingDetected = true` → `JAM` badge
- `currentTracks` shown in the attribute info panel

**Default values:**

| Field | Default |
|-------|---------|
| `operationalMode` | `"Standby"` |
| `trackCapacity` | `100` |
| `emconActive` | `false` |
| `transmitPowerPct` | `100.0` |
| `iffEnabled` | `true` |
| `iffMode` | `"Mode-3"` |

---

### 4.3 Maintenance Attributes

Health monitoring and logistics parameters.

```json
{
    "maintenance": {
        "systemHealth":    "Nominal",
        "healthPct":       98.5,
        "transmitterOk":   true,
        "receiverOk":      true,
        "antennaOk":       true,
        "coolingSysOk":    true,
        "powerSupplyOk":   true,
        "lastServiceDate": "2025-11-01",
        "nextServiceDate": "2026-05-01",
        "operatingHours":  1240,
        "serviceIntervalH":500,
        "mtbfHours":       2000.0,
        "mttrHours":       4.0,
        "maintenanceNotes":"All systems nominal."
    }
}
```

| Field | Type | Unit | Allowed Values / Notes |
|-------|------|------|------------------------|
| `systemHealth` | string | – | `Nominal` \| `Degraded` \| `Critical` \| `Failed` |
| `healthPct` | float | % | 0–100; drives health colour in PPI panel |
| `transmitterOk` | bool | – | Component status |
| `receiverOk` | bool | – | Component status |
| `antennaOk` | bool | – | Component status |
| `coolingSysOk` | bool | – | Component status |
| `powerSupplyOk` | bool | – | Component status |
| `lastServiceDate` | string | – | ISO 8601 date (`YYYY-MM-DD`) |
| `nextServiceDate` | string | – | ISO 8601 date (`YYYY-MM-DD`) |
| `operatingHours` | int | h | Total operating hours |
| `serviceIntervalH` | int | h | Hours between required service |
| `mtbfHours` | float | h | Mean time between failures |
| `mttrHours` | float | h | Mean time to repair |
| `maintenanceNotes` | string | – | Free text |

**Health colour in PPI:**
- `healthPct >= 80` → green
- `50 <= healthPct < 80` → yellow
- `healthPct < 50` → red

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
            "design":      { ... },
            "operational": { ... },
            "maintenance": { ... }
        }
    }
}
```

The `range` field (metres) is the legacy range field; `design.maxRangeKm`
takes precedence when `radarAttributes` is present.

---

## 6. PPI View – Attribute Display

The PPI (Plan Position Indicator) widget renders the following attribute-derived
information in real time:

| PPI Element | Source Attribute |
|-------------|-----------------|
| Range ring labels (`N km`) | `design.maxRangeKm` |
| Operational mode pill | `operational.operationalMode` |
| Frequency info panel | `design.frequencyMHz` |
| Azimuth coverage | `design.azimuthCovDeg` |
| Elevation range | `design.elevationMinDeg` / `elevationMaxDeg` |
| Track count | `operational.currentTracks` / `trackCapacity` |
| Transmit power | `operational.transmitPowerPct` |
| IFF mode | `operational.iffEnabled` / `iffMode` |
| Health bar colour | `maintenance.healthPct` |
| EMCON badge | `operational.emconActive` |
| JAM badge | `operational.jammingDetected` |

---

## 7. Operator Attribute Editing

An operator can edit attributes through the GUI at any time:

1. **Right-click** a RADAR icon on the map canvas.
2. Select **"Attributes"** from the context menu.
3. A tabbed dialog opens with three tabs (Design / Operational / Maintenance).
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
        "design":      {"maxRangeKm": 150.0, "frequencyMHz": 2900.0,
                        "azimuthCovDeg": 360.0, "radarType": "Surveillance"},
        "operational": {"operationalMode": "Active"},
        "maintenance": {"healthPct": 100.0, "systemHealth": "Nominal"}
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
| `RadarView/radarattributes.h` | `DesignAttributes`, `OperationalAttributes`, `MaintenanceAttributes`, `RadarAttributes` structs + JSON serialization |
| `RadarView/radarmanager.h/cpp` | `RadarManager` – owns all `Radar` objects + detection lists |
| `RadarView/radarattributesdialog.h/cpp` | Tabbed attribute editor dialog |
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
