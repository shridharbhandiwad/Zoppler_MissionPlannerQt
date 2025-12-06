# VistarPlanner - Quick Start Guide

## What Was Fixed

The drone data display issue has been resolved. Previously, the application was receiving data from the backend but couldn't display it because the MapDisplay components were missing. All necessary components have now been implemented.

## How It Works

1. **Network Reception**: UDP data arrives at port 8888 (multicast 225.0.0.1)
2. **JSON Parsing**: CNetworkInterface parses the JSON and emits signal
3. **Display Processing**: CMapCanvas receives data via slotUpdateObject()
4. **Object Management**: Creates/updates CVistarObject instances
5. **Visualization**: QGIS renders objects on the map canvas

## Running the Application

### Prerequisites
Ensure you have:
- Qt development environment
- QGIS libraries installed
- C++17 compiler

### Build Steps
```bash
cd /workspace
qmake VistarPlanner.pro
make
```

### Run
```bash
../../bin/VistarPlanner
```

## Testing Without Backend

If you want to test without your actual backend:

### Step 1: Start the Application
Run VistarPlanner and click the "INITIALIZE" button.

### Step 2: Send Test Data
In a separate terminal:
```bash
cd /workspace
python3 test_drone_sender.py
```

Select option 8 for a full test sequence, or option 3 for a continuously moving drone.

### Step 3: Observe
- Objects appear on the map with color coding
- Status bar shows latest updates
- Objects update in real-time as data arrives

## Testing With Your Backend

Your backend just needs to send JSON data to UDP multicast address 225.0.0.1:8888.

### Minimal Object Update
```json
{
    "ID": "DRONE_001",
    "CLASS": 1,
    "LAT": 28.6139,
    "LON": 77.2090,
    "ALT": 100.0
}
```

### Full Object Update
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

## Object Classes (CLASS field)

| Value | Type | Display Color |
|-------|------|---------------|
| 1 | DRONE | Green |
| 2 | DRONE_SWARM | Light Green |
| 3 | FIGHTER | Red |
| 4 | UAV | Light Blue |
| 5 | RADAR | Orange |
| 6 | LAUNCHER | Purple |
| 7 | MISSILE | Yellow |

## UI Controls

### Buttons
- **Import Map**: Load a TIFF/GeoTIFF raster map
- **Object Dropdown**: Select object type to create
- **Mark**: Enable object creation mode (click on map to place)
- **INITIALIZE**: Prepare all objects for display
- **START**: Send start event to backend
- **STOP**: Send stop event to backend

### Status Bar
Shows real-time updates: `Updated: DRONE_001 [Class:1] Lat:28.613900 Lon:77.209000`

## Troubleshooting

### No Objects Appearing
1. Check that data is being received (look at application debug output)
2. Click "INITIALIZE" button
3. Verify JSON format is correct
4. Check network connectivity to multicast group

### Objects Not Updating
1. Ensure each update has the same "ID" field for the same object
2. Check that LAT/LON values are valid
3. Verify UDP packets are reaching the application

### Application Won't Build
1. Verify QGIS libraries are installed
2. Check QGIS_PREFIX path in VistarPlanner.pro
3. Ensure Qt development tools are available

## Debug Output

The application prints debug messages:
```
[CMapCanvas] Initialized successfully
[CMapCanvas] Received data: {"ID":"DRONE_001",...}
[CMapCanvas] Creating new object: DRONE_001 class: 1
```

Enable more verbose output by setting:
```bash
export QT_LOGGING_RULES="*.debug=true"
```

## Data Flow Summary

```
Backend → UDP (225.0.0.1:8888) → CNetworkInterface → 
  → signalUpdateObject → CMapCanvas::slotUpdateObject() → 
    → CVistarObject::Update() → QGIS Layer → Map Display
```

## Example Backend Integration (Python)

```python
import socket
import json

def send_drone_update(drone_id, lat, lon, alt):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)
    
    data = {
        "ID": drone_id,
        "CLASS": 1,
        "LAT": lat,
        "LON": lon,
        "ALT": alt
    }
    
    message = json.dumps(data).encode('utf-8')
    sock.sendto(message, ('225.0.0.1', 8888))
    sock.close()

# Usage
send_drone_update("DRONE_001", 28.6139, 77.2090, 100.0)
```

## Next Steps

1. Build and run the application
2. Test with the provided test_drone_sender.py script
3. Integrate with your actual backend
4. Customize object colors/icons as needed (see cvistarobject.cpp)
5. Add custom map layers via "Import Map" button

## Support

For issues or questions:
1. Check debug output for error messages
2. Verify JSON data format matches specification
3. Test with test_drone_sender.py first
4. Review DRONE_DATA_DISPLAY_FIX.md for technical details

## Summary

The drone data display system is now fully functional. The missing MapDisplay components have been implemented, and the application can now receive, process, and display drone data in real-time on a QGIS-based map canvas.
