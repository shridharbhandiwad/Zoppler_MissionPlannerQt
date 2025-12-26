# Vistar Routes and Objects Auto-Save Feature

## Overview

The VistarPlanner now includes automatic scenario saving and loading functionality for all vistar routes with their attached objects. This feature ensures that your work is preserved between sessions and can be saved/loaded manually at any time.

## Features Implemented

### 1. **Automatic Saving**
- The system automatically saves your current scenario whenever you:
  - Add a new object (DRONE, FIGHTER, UAV, RADAR, LAUNCHER, etc.)
  - Complete creating a route
  - Delete an object or route
  - Update an object's position
  - Modify a route's waypoints

### 2. **Automatic Loading**
- When you start the application, it automatically loads the last saved scenario
- The auto-load happens 500ms after initialization to ensure the map is ready
- If no previous scenario exists, the application starts with a clean slate

### 3. **Manual Save/Load**
- **Save Scenario Button**: Allows you to manually save the current scenario to a custom location
- **Load Scenario Button**: Allows you to load a previously saved scenario from any location
- Status bar provides feedback on save/load operations

## How It Works

### Scenario Storage Format

Scenarios are saved as JSON files containing:

```json
{
  "name": "Auto-saved Scenario",
  "description": "Automatically saved vistar routes and objects",
  "createdDate": "2025-12-26T10:30:00",
  "objects": [
    {
      "id": "RADAR_1",
      "type": "RADAR",
      "latitude": 28.5000,
      "longitude": 77.2000,
      "altitude": 100.0,
      "additionalData": {
        "parent": "",
        "childId": 0
      }
    }
  ],
  "routes": [
    {
      "id": "ROUTE_1",
      "name": "ROUTE_1",
      "waypoints": [
        {"latitude": 28.5, "longitude": 77.2},
        {"latitude": 28.6, "longitude": 77.3}
      ],
      "altitudes": [1000.0, 1000.0]
    }
  ],
  "metadata": {
    "version": "1.0",
    "autoSaved": true
  }
}
```

### File Locations

- **Auto-save location**: The application saves scenarios automatically to:
  - Windows: `%APPDATA%/VistarPlanner/scenarios/current_scenario.json`
  - Linux: `~/.local/share/VistarPlanner/scenarios/current_scenario.json`
  
- **Manual save**: You can save to any location using the "Save Scenario" button

## Usage Guide

### Starting the Application

1. Launch VistarPlanner
2. The application will automatically load the last saved scenario (if exists)
3. All routes and objects will be restored to their previous state

### Working with the Application

1. **Add Objects**: Click on object type (DRONE, FIGHTER, etc.) and place on map
   - The scenario is auto-saved after adding each object
   
2. **Create Routes**: Select ROUTE and click waypoints on the map
   - The scenario is auto-saved when route is completed or double-clicked to finish
   
3. **Modify Objects**: Right-click on objects to update position
   - The scenario is auto-saved after modifications
   
4. **Delete Objects/Routes**: Right-click and select "Delete"
   - The scenario is auto-saved after deletion

### Manual Save/Load

#### Save Scenario:
1. Click the **"Save Scenario"** button (orange button in top-left area)
2. Choose a location and filename
3. The scenario will be saved as a JSON file
4. Status bar will confirm the save operation

#### Load Scenario:
1. Click the **"Load Scenario"** button (cyan button below Save)
2. Select a previously saved scenario JSON file
3. All current objects and routes will be cleared
4. The selected scenario will be loaded
5. Status bar will confirm the load operation

## Technical Details

### Key Components

1. **CMapCanvas Methods**:
   - `saveCurrentScenario(filePath)`: Saves current state to file
   - `loadScenario(filePath)`: Loads scenario from file
   - `autoSaveScenario()`: Auto-saves to default location
   - `autoLoadScenario()`: Auto-loads from default location
   - `createScenarioFromCurrentState()`: Creates scenario structure
   - `loadScenarioToCanvas()`: Restores scenario to canvas

2. **CScenarioManager**: Handles JSON serialization/deserialization

3. **Scenario Data Structures**:
   - `ScenarioObject`: Stores object data (position, type, etc.)
   - `ScenarioRoute`: Stores route waypoints and altitudes
   - `Scenario`: Complete scenario with metadata

### What Gets Saved

✅ **Saved**:
- All vistar objects (DRONE, FIGHTER, UAV, RADAR, LAUNCHER, DRONE_SWARM)
- Object positions (latitude, longitude, altitude)
- All routes with waypoints
- Parent-child relationships (for missiles attached to launchers/UAVs)

❌ **Not Saved**:
- Child objects (missiles) - these are automatically regenerated based on parent type
- Temporary UI state
- Network connections
- Map layers and zoom level

### Child Object Regeneration

When loading a scenario:
- **LAUNCHER**: Automatically creates 12 child missiles
- **UAV**: Automatically creates 4 child missiles  
- **FIGHTER**: No child missiles by default

This matches the behavior when objects are originally created.

## Troubleshooting

### Scenario Not Loading on Startup
- Check if the scenarios directory exists
- Verify the file `current_scenario.json` exists in the scenarios folder
- Check console output for error messages

### Objects Missing After Load
- Ensure the JSON file is properly formatted
- Child objects (missiles) are not saved but regenerated automatically
- Check that object types match the expected enum values

### Auto-Save Not Working
- Verify you have write permissions to the scenarios directory
- Check console output for save errors
- Manual save can be used as an alternative

## Benefits

1. **Data Persistence**: Never lose your work when closing the application
2. **Version Control**: Save multiple scenarios with different names
3. **Quick Setup**: Instantly restore complex scenarios with many objects
4. **Backup**: Keep multiple versions of your scenarios
5. **Sharing**: Share scenario files with other users

## Future Enhancements (Possible)

- Scenario browser/manager UI
- Scenario templates
- Scenario merging
- Export/Import to other formats
- Scenario versioning and history
- Cloud sync capability

---

For any issues or questions, please refer to the main documentation or contact support.
