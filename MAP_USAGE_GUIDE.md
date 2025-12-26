# Map Loading - Quick Start Guide

## How to Load a Map

1. **Launch the Application**
   ```bash
   ./build/VistarPlanner
   ```

2. **Click the "Import Map" Button**
   - Located in the top-left corner of the window (purple gradient button)
   - This opens a file selection dialog

3. **Select Your Map File**
   - Supported formats: TIFF, GeoTIFF, and other GDAL-supported raster formats
   - Navigate to your map file and click "Open"

4. **View the Map**
   - The map will be displayed in the canvas
   - Automatically zoomed to show the full extent of the loaded map

## Map Controls

### Mouse Interaction
- **Pan**: Click and drag to move around the map
- **Zoom**: Use mouse wheel to zoom in/out
- **Coordinates**: Move mouse over map to see coordinates in status bar (bottom)

### Coordinates
- Format: Longitude/Latitude in decimal degrees
- Example: "Lon: 77.209000, Lat: 28.613900"
- Coordinate system: WGS84 (EPSG:4326)

## Getting Sample Maps

### Option 1: Natural Earth Data (Free)
```bash
# Download Natural Earth I with Shaded Relief
wget https://www.naturalearthdata.com/http//www.naturalearthdata.com/download/10m/raster/NE1_HR_LC_SR_W.zip
unzip NE1_HR_LC_SR_W.zip
# Open the .tif file in VistarPlanner
```

### Option 2: Create Test Map with GDAL
```bash
# Install GDAL tools
sudo apt-get install gdal-bin

# Create a simple test GeoTIFF
gdal_create -of GTiff -outsize 1000 1000 -a_srs EPSG:4326 \
    -a_ullr -180 90 180 -90 -burn 100 test_map.tif
```

### Option 3: OpenStreetMap Export
1. Visit: https://www.openstreetmap.org/export
2. Select area of interest
3. Export as image
4. Convert to GeoTIFF using GDAL if needed

## Troubleshooting

### Map Not Displaying
- **Check file format**: Ensure the file is a valid raster format
- **Check file path**: Make sure the file isn't corrupted
- **Check console output**: Look for QGIS error messages in terminal

### Error: "Failed to load map file"
- File may not be a valid GeoTIFF
- File may be corrupted
- Format may not be supported by QGIS version

### Coordinates Not Showing
- Ensure mouse is over the map canvas (black area)
- Check status bar at bottom of window

## Adding Objects to Map

1. **Select Object Type**
   - Use the dropdown menu (shows DRONE, FIGHTER, etc.)

2. **Click "Mark" Button**
   - This activates object placement mode

3. **Click on Map**
   - Click desired location to place object
   - Object data sent to network interface

## Network Integration

The map canvas receives real-time object updates via UDP:
- **Protocol**: UDP Multicast
- **Address**: 225.0.0.1:8888
- **Format**: JSON

Example object update:
```json
{
    "STREAM": "Object",
    "TYPE": "Update",
    "ID": "DRONE_001",
    "CLASS": 1,
    "LAT": 28.6139,
    "LON": 77.2090,
    "ALT": 100.0
}
```

## Scenario Management

Load pre-configured scenarios with objects and routes:

1. **Click "Scenario" Button** (orange button next to Import Map)
2. **Select "Load Scenario"**
3. **Choose from**:
   - Sample scenarios (pre-loaded)
   - Your saved scenarios
4. **Objects and routes load automatically on map**

## Tips

- **Performance**: Larger maps (>100MB) may take longer to load
- **Quality**: Higher resolution maps provide better detail when zoomed in
- **Coverage**: Global maps show more area but less detail
- **CRS**: All maps automatically reprojected to WGS84 if needed

## Keyboard Shortcuts (QGIS Standard)

While these work with QGIS map canvas:
- `+` / `-`: Zoom in/out
- `Ctrl+F`: Fit map to window
- `Ctrl+R`: Refresh map

## Next Steps

1. Load a map file
2. Test object placement
3. Load a scenario with pre-configured objects
4. Connect external data source (UDP) for real-time updates

For developers: See `MAP_LOADING_FIX.md` for implementation details.
