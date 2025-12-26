# Zoppler_MissionPlannerQt
Mission Planner

## Overview
VistarPlanner is a Qt-based mission planning application for drone and tactical object visualization. It uses QGIS for map rendering and UDP networking for real-time data updates.

## ✅ FIXED: Map Loading Issue (Dec 2024)

The map loading functionality has been completely implemented! You can now:
- ✅ Load GeoTIFF and TIFF map files via "Import Map" button
- ✅ View maps with full QGIS rendering capabilities
- ✅ Pan and zoom with mouse controls
- ✅ See real-time coordinates in status bar
- ✅ Place objects on the map
- ✅ Visualize routes and waypoints

**Quick Start**: Click the purple "Import Map" button, select a GeoTIFF file, and start planning!

📖 **See [MAP_LOADING_FIX.md](MAP_LOADING_FIX.md) for technical details**
📖 **See [MAP_USAGE_GUIDE.md](MAP_USAGE_GUIDE.md) for user guide**

## NEW: Scenario Management Feature 🎯

The application now includes a comprehensive **Scenario Management System** that allows you to save and load complete mission scenarios including objects, routes, and configurations.

### Quick Start
1. Click the **"Scenario"** button (orange button next to Import Map)
2. Choose **"Load Scenario"** to load pre-made scenarios or your saved scenarios
3. Choose **"Save Scenario"** to save your current setup

### Features
- ✅ Load and save complete scenarios (objects + routes)
- ✅ Three pre-configured sample scenarios included
- ✅ JSON-based scenario format
- ✅ Easy scenario sharing and reuse

📖 **See [SCENARIO_FEATURE_README.md](SCENARIO_FEATURE_README.md) for detailed documentation**

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
├── Scenario Management/        # NEW: Scenario system
│   ├── cscenariomanager.h/cpp          # Scenario manager
│   ├── cloadscenariodialog.h/cpp/ui    # Load dialog
│   └── csavescenariodialog.h/cpp/ui    # Save dialog
├── sample_scenarios/           # NEW: Sample scenario templates
│   ├── Basic_Defense_Setup.json
│   ├── Air_Patrol_Mission.json
│   └── Multi_Target_Engagement.json
├── MapDisplay/                 # Display components
│   ├── cmapcanvas.h/cpp       # Main map canvas
│   ├── cvistarobject.h/cpp    # Object visualization
│   ├── cvistarroute.h/cpp     # Route visualization
│   ├── cmaptoolselectobjects.h/cpp  # Map tools
│   └── qgspointxyz.h/cpp      # 3D coordinates
└── test_drone_sender.py       # Test script
```
