# Scenario Management Feature

## Overview

The Scenario Management feature allows you to save and load complete scenarios including objects (drones, fighters, radars, launchers, etc.) and their associated routes. This makes it easy to set up complex mission scenarios quickly and share them with others.

## Features

### 1. **Scenario Dropdown Button**
- A new orange "Scenario" button has been added to the main interface
- Located next to the "Import Map" button
- Click the button to access the dropdown menu with options:
  - **Load Scenario**: Load an existing or sample scenario
  - **Save Scenario**: Save the current scenario

### 2. **Load Scenario**
When you click "Load Scenario", a dialog opens with two tabs:

#### Saved Scenarios Tab
- Browse previously saved scenarios from your scenarios directory
- Double-click to load or select and click "Load"
- Use "Browse for File..." to load a scenario from any location

#### Sample Scenarios Tab
- Three pre-configured sample scenarios are available:
  1. **Basic Defense Setup**: A simple air defense scenario with radar, launcher, and patrol route
  2. **Air Patrol Mission**: Multiple UAVs on patrol routes with surveillance coverage
  3. **Multi-Target Engagement**: Complex scenario with multiple threats and defense assets
- Hover over each scenario to see its description
- Double-click or select and click "Load"

### 3. **Save Scenario**
When you click "Save Scenario", a dialog opens allowing you to:
- Enter a scenario name (required)
- Add an optional description
- Choose save location (defaults to application data directory)
- Scenario will be saved in JSON format

## File Structure

### Scenario JSON Format
Scenarios are saved as JSON files with the following structure:

```json
{
    "name": "Scenario Name",
    "description": "Scenario description",
    "createdDate": "2025-12-26T10:00:00",
    "objects": [
        {
            "id": "OBJECT_001",
            "type": "DRONE|FIGHTER|UAV|RADAR|LAUNCHER|MISSILE|DRONE_SWARM",
            "latitude": 28.5,
            "longitude": 77.2,
            "altitude": 5000.0,
            "additionalData": {
                // Custom properties specific to object type
            }
        }
    ],
    "routes": [
        {
            "id": "ROUTE_001",
            "name": "Route Name",
            "waypoints": [
                {
                    "latitude": 28.5,
                    "longitude": 77.2,
                    "altitude": 5000.0
                }
            ],
            "additionalData": {
                // Custom route properties
            }
        }
    ],
    "metadata": {
        "author": "System",
        "version": "1.0",
        "difficulty": "easy|medium|hard"
    }
}
```

## Sample Scenarios

### 1. Basic Defense Setup
**Location**: `sample_scenarios/Basic_Defense_Setup.json`

**Contents**:
- 1 Radar (RADAR_001) - Active surveillance
- 1 Launcher (LAUNCHER_001) - 8 missiles ready
- 1 Fighter (FIGHTER_001) - Callsign ALPHA-1
- 1 Combat Air Patrol (CAP) route

**Use Case**: Training or testing basic air defense concepts

### 2. Air Patrol Mission
**Location**: `sample_scenarios/Air_Patrol_Mission.json`

**Contents**:
- 2 UAVs (REAPER-1 and REAPER-2) - 12 hour endurance
- 1 Drone Swarm - 10 drones in V formation
- 2 Patrol routes (North and South)

**Use Case**: Surveillance and reconnaissance operations

### 3. Multi-Target Engagement
**Location**: `sample_scenarios/Multi_Target_Engagement.json`

**Contents**:
- 2 Radars (long-range and medium-range)
- 2 Launchers (12 and 10 missiles)
- 2 Fighters (VIPER-1 and VIPER-2)
- 2 Routes (Intercept Vector and CAP Station)

**Use Case**: Complex air defense with multiple assets and threats

## Default Scenario Directory

Scenarios are stored in your application data directory:
- **Windows**: `C:\Users\<username>\AppData\Local\VistarPlanner\scenarios\`
- **Linux**: `~/.local/share/VistarPlanner/scenarios/`
- **macOS**: `~/Library/Application Support/VistarPlanner/scenarios/`

## Usage Instructions

### Loading a Sample Scenario
1. Click the "Scenario" button in the top-left area of the map canvas
2. Select "Load Scenario" from the dropdown menu
3. Switch to the "Sample Scenarios" tab
4. Select one of the three sample scenarios
5. Click "Load" or double-click the scenario
6. The scenario will be loaded onto the map with all objects and routes

### Saving Your Current Scenario
1. Set up your objects and routes on the map
2. Click the "Scenario" button
3. Select "Save Scenario" from the dropdown menu
4. Enter a name for your scenario (e.g., "My Defense Plan")
5. Optionally add a description
6. Choose a save location or use the default
7. Click "Save"
8. Your scenario will be saved and can be loaded later

### Loading a Previously Saved Scenario
1. Click the "Scenario" button
2. Select "Load Scenario" from the dropdown menu
3. In the "Saved Scenarios" tab, select your scenario
4. Click "Load" or double-click the scenario
5. Or click "Browse for File..." to load from any location

## Technical Details

### New Classes Added

1. **CScenarioManager**: Core class for scenario management
   - Handles saving/loading scenarios to/from JSON
   - Provides sample scenario generation
   - Manages scenario directory

2. **CLoadScenarioDialog**: Dialog for loading scenarios
   - Tab-based interface for saved and sample scenarios
   - File browser integration
   - Preview and description display

3. **CSaveScenarioDialog**: Dialog for saving scenarios
   - Input validation
   - Custom save location selection
   - Overwrite protection

### Files Added

- `cscenariomanager.h/cpp`: Scenario manager implementation
- `cloadscenariodialog.h/cpp/ui`: Load scenario dialog
- `csavescenariodialog.h/cpp/ui`: Save scenario dialog
- `sample_scenarios/*.json`: Sample scenario templates

### Integration Points

The scenario feature integrates with:
- Main window (`CVistarPlanner`)
- Map canvas (`CMapCanvas`) - for loading/saving objects and routes
- Network interface - scenarios can include network-synced objects

## Future Enhancements

Potential improvements for the scenario system:
- Scenario validation and error checking
- Import/export to different formats
- Scenario templates with parameters
- Scenario versioning and migration
- Collaborative scenario editing
- Cloud-based scenario sharing
- Scenario diff and merge tools

## Troubleshooting

### Scenario Won't Load
- Check that the JSON file is valid
- Verify all required fields are present
- Check file permissions

### Can't Save Scenario
- Ensure the scenarios directory exists and is writable
- Check for invalid characters in scenario name
- Verify sufficient disk space

### Objects Not Appearing After Load
- Ensure mapCanvas has appropriate methods to receive objects
- Check that object types match expected types
- Verify coordinate ranges are valid

## Support

For issues or questions about the scenario management feature, please refer to the main application documentation or contact the development team.
