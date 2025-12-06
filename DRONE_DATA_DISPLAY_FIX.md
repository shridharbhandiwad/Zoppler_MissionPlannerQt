# Drone Data Display Fix - Documentation

## Problem
The VistarPlanner application was receiving drone data from the backend via UDP network interface, but the data was not being displayed. The issue was that the MapDisplay components referenced in the code were missing.

## Solution
Created the complete MapDisplay component system to receive, process, and display drone data on a QGIS-based map canvas.

## Components Created

### 1. Core Display Classes

#### `CMapCanvas` (MapDisplay/cmapcanvas.h/cpp)
- **Main display component** - The central widget that manages the map display
- **Key Features:**
  - Initializes QGIS map canvas for visualization
  - Receives JSON data through `slotUpdateObject(QJsonDocument)` slot
  - Manages collection of drone objects and routes
  - Handles different data types: Objects, Routes, and Events
  - Provides real-time status updates via signals

#### `CVistarObject` (MapDisplay/cvistarobject.h/cpp)
- Represents individual objects (drones, fighters, UAVs, etc.)
- **Features:**
  - Updates position, heading, velocity from JSON data
  - Creates and manages QGIS vector layers for display
  - Color-coded by object class (green for drones, red for fighters, etc.)
  - Shows/hides objects dynamically

#### `CVistarRoute` (MapDisplay/cvistarroute.h/cpp)
- Displays flight paths and routes
- **Features:**
  - Manages waypoint collections
  - Creates line geometries for route visualization
  - Supports custom colors
  - Dynamic updates as waypoints change

#### `CMapToolSelectObjects` (MapDisplay/cmaptoolselectobjects.h/cpp)
- Map interaction tool for creating objects
- Handles mouse clicks to place new objects on the map
- Emits signals for object creation at clicked locations

#### `QgsPointXYZ` (MapDisplay/qgspointxyz.h/cpp)
- Helper class for 3D coordinate handling
- Extends QGIS QgsPoint with Z-coordinate (altitude) support

## Data Flow

```
Network (UDP) → CNetworkInterface → JSON Parser → slotUpdateObject() → 
    → CMapCanvas → CVistarObject/CVistarRoute → QGIS Layers → Map Display
```

### JSON Data Format

#### Object Update
```json
{
    "STREAM": "Object",
    "TYPE": "Update",
    "ID": "DRONE_001",
    "CLASS": 1,
    "LAT": 28.6139,
    "LON": 77.2090,
    "ALT": 100.0,
    "HEADING": 45.0,
    "VELOCITY": 25.5
}
```

#### Route Update
```json
{
    "STREAM": "Route",
    "TYPE": "Update",
    "ID": "ROUTE_001",
    "WAYPOINTS": [
        {"LAT": 28.6139, "LON": 77.2090, "ALT": 100},
        {"LAT": 28.6239, "LON": 77.2190, "ALT": 150}
    ],
    "COLOR": "#00FFFF"
}
```

#### Event Messages
```json
{
    "STREAM": "Event",
    "TYPE": "Start"  // or "Stop", "Clear"
}
```

## Object Classes

The system supports different object types defined in `globalConstants.h`:

| Class ID | Type | Color |
|----------|------|-------|
| 1 | DRONE | Green (0, 255, 0) |
| 2 | DRONE_SWARM | Light Green (0, 200, 100) |
| 3 | FIGHTER | Red (255, 0, 0) |
| 4 | UAV | Light Blue (0, 150, 255) |
| 5 | RADAR | Orange (255, 165, 0) |
| 6 | LAUNCHER | Purple (128, 0, 128) |
| 7 | MISSILE | Yellow (255, 255, 0) |
| 8 | ROUTE | Cyan |

## Network Configuration

- **Protocol**: UDP Multicast
- **Multicast Group**: 225.0.0.1
- **Port**: 8888
- **Data Format**: JSON (UTF-8 encoded)

## Key Methods

### CMapCanvas::slotUpdateObject(QJsonDocument doc)
This is the **critical method** that was missing and causing the display issue. It:
1. Parses incoming JSON document
2. Determines data type (Object/Route/Event)
3. Creates or updates corresponding display objects
4. Refreshes the map canvas
5. Emits status updates to the UI

### CVistarObject::Update(QJsonObject jsonData)
Updates object properties from JSON:
- Position (LAT, LON, ALT)
- Heading and velocity
- Object ID and class
- Triggers layer refresh for visual update

## Building the Project

```bash
cd /workspace
qmake VistarPlanner.pro
make
```

The executable will be created at: `../../bin/VistarPlanner`

## Testing

Use the provided `test_drone_sender.py` script to test the system:

```bash
chmod +x test_drone_sender.py
python3 test_drone_sender.py
```

The test script provides multiple test modes:
1. Single drone update
2. Multiple drones
3. Moving drone (continuous simulation)
4. Various object types
5. Route visualization
6. Event messages
7. Full test sequence

## Usage

1. **Start the Application**: Run VistarPlanner
2. **Initialize**: Click "INITIALIZE" button to prepare objects for display
3. **Send Data**: Use your backend or test script to send drone data
4. **View Updates**: The status bar shows latest updates
5. **Create Objects**: Select object type from dropdown, click "Mark", then click on map

## Signal/Slot Connections

The fix establishes this connection in `cvistarplanner.cpp`:
```cpp
connect(_m_networkInterface, SIGNAL(signalUpdateObject(QJsonDocument)),
        ui->mapCanvas, SLOT(slotUpdateObject(QJsonDocument)));
```

This connection was defined in the code but the receiving slot (`slotUpdateObject`) didn't exist until now.

## Debugging

Enable Qt debug output to see processing messages:
- Object creation: `[CMapCanvas] Creating new object: DRONE_001 class: 1`
- Data received: `[CMapCanvas] Received data: {...}`
- Updates: `Updated: DRONE_001 [Class:1] Lat:28.613900 Lon:77.209000`

## Files Modified/Created

**Created:**
- MapDisplay/cmapcanvas.h
- MapDisplay/cmapcanvas.cpp
- MapDisplay/cvistarobject.h
- MapDisplay/cvistarobject.cpp
- MapDisplay/cvistarroute.h
- MapDisplay/cvistarroute.cpp
- MapDisplay/cmaptoolselectobjects.h
- MapDisplay/cmaptoolselectobjects.cpp
- MapDisplay/qgspointxyz.h
- MapDisplay/qgspointxyz.cpp
- test_drone_sender.py
- DRONE_DATA_DISPLAY_FIX.md (this file)

**Existing files referenced:**
- cnetworkinterface.h/cpp (already receiving data correctly)
- cvistarplanner.h/cpp (already had signal/slot connection)
- globalConstants.h (defines object classes)

## Summary

The issue was that the display layer was completely missing. The network interface was functioning correctly and receiving data, but the `CMapCanvas::slotUpdateObject()` method that was supposed to handle the received data didn't exist because the entire MapDisplay module was missing. 

This fix implements the complete display system, allowing drone data to be properly visualized on the map canvas with real-time updates, color coding by object type, and support for multiple simultaneous objects.
