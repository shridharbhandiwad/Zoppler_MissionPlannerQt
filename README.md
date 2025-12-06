# Zoppler_MissionPlannerQt
Mission Planner

## Overview
VistarPlanner is a Qt-based mission planning application for drone and tactical object visualization. It uses QGIS for map rendering and UDP networking for real-time data updates.

## Recent Fix: Drone Data Display Issue

**Problem**: Drone data was being received from the backend but not displayed on the map.

**Solution**: Implemented the complete MapDisplay component system including:
- CMapCanvas: Main display widget with slotUpdateObject() to receive and process data
- CVistarObject: Drone/object representation and visualization
- CVistarRoute: Route/path visualization
- CMapToolSelectObjects: Interactive object placement
- QgsPointXYZ: 3D coordinate handling

See [DRONE_DATA_DISPLAY_FIX.md](DRONE_DATA_DISPLAY_FIX.md) for detailed documentation.

## Building

Requirements:
- Qt 5.x or 6.x
- QGIS libraries (qgis_core, qgis_gui)
- C++17 compiler

```bash
qmake VistarPlanner.pro
make
```

## Testing

Test the drone data display system:

```bash
python3 test_drone_sender.py
```

This will send test drone data via UDP multicast (225.0.0.1:8888).

## Network Protocol

- Protocol: UDP Multicast
- Address: 225.0.0.1
- Port: 8888
- Format: JSON

Example drone update:
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

## Object Classes

1. DRONE - Green
2. DRONE_SWARM - Light Green
3. FIGHTER - Red
4. UAV - Light Blue
5. RADAR - Orange
6. LAUNCHER - Purple
7. MISSILE - Yellow
8. ROUTE - Cyan

## Project Structure

```
├── main.cpp                    # Application entry point
├── cvistarplanner.h/cpp       # Main window
├── cnetworkinterface.h/cpp    # UDP networking
├── globalConstants.h           # Constants and enums
├── MapDisplay/                 # Display components
│   ├── cmapcanvas.h/cpp       # Main map canvas
│   ├── cvistarobject.h/cpp    # Object visualization
│   ├── cvistarroute.h/cpp     # Route visualization
│   ├── cmaptoolselectobjects.h/cpp  # Map tools
│   └── qgspointxyz.h/cpp      # 3D coordinates
└── test_drone_sender.py       # Test script
```
