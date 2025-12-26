# Scenario Feature - Quick Reference Guide

## New UI Element

```
┌─────────────────────────────────────────────────────────────┐
│  [Import Map]  [Scenario ▼]    [Object Selection ▼] [Mark] │
│   (Purple)      (Orange)           (White)          (Blue)  │
└─────────────────────────────────────────────────────────────┘
```

## Scenario Dropdown Menu

When you click **[Scenario ▼]**, a popup menu appears:

```
┌────────────────────┐
│ Load Scenario      │
│ Save Scenario      │
└────────────────────┘
```

## Quick Actions

### Load a Scenario
**Shortcut**: Click `Scenario ▼` → `Load Scenario`

**Options**:
1. **Saved Scenarios Tab**:
   - View list of previously saved scenarios
   - Double-click to load
   - Click "Browse" to find file elsewhere

2. **Sample Scenarios Tab**:
   - Basic Defense Setup
   - Air Patrol Mission
   - Multi-Target Engagement

### Save Current Scenario
**Shortcut**: Click `Scenario ▼` → `Save Scenario`

**Required**:
- Scenario Name (no special characters: `< > : " / \ | ? *`)

**Optional**:
- Description
- Custom save location

## Scenario Contents

A scenario saves:
- ✓ All Drones
- ✓ All Drone Swarms
- ✓ All Fighters
- ✓ All UAVs
- ✓ All Radars
- ✓ All Launchers
- ✓ All Missiles
- ✓ All Routes
- ✓ Position data (Lat/Lon/Alt)
- ✓ Custom properties

## File Format

- **Extension**: `.json` or `.scenario`
- **Location**: `AppData/VistarPlanner/scenarios/`
- **Format**: Human-readable JSON

## Sample Workflow

### Creating a New Scenario

1. Place objects on map (Drones, UAVs, etc.)
2. Define routes and waypoints
3. Click `Scenario ▼` → `Save Scenario`
4. Enter name: "My Defense Plan"
5. Add description: "Northern border defense"
6. Click `Save`
7. Confirmation: "Scenario 'My Defense Plan' has been saved successfully"

### Loading an Existing Scenario

1. Click `Scenario ▼` → `Load Scenario`
2. Select "Air Patrol Mission" from Sample Scenarios
3. Click `Load`
4. Map automatically populates with:
   - 2 UAVs in surveillance positions
   - 1 Drone Swarm (10 drones)
   - 2 Patrol routes
5. Confirmation: "Sample scenario 'Air Patrol Mission' has been loaded successfully"

## Common Use Cases

### Training
- Load pre-built sample scenarios
- Practice with different tactical situations
- Learn object placement and routing

### Mission Planning
- Save different mission variants
- Compare alternative approaches
- Share plans with team members

### Testing
- Create test scenarios
- Verify system behavior
- Reproduce specific situations

## Tips & Tricks

1. **Naming Convention**: Use descriptive names
   - Good: "North_Border_Defense_2024"
   - Bad: "scenario1"

2. **Descriptions**: Add details about scenario purpose
   - Good: "3-layer defense with CAP support, suitable for high-threat areas"
   - Bad: "defense"

3. **Organization**: Use consistent naming for easy searching
   - Prefix by type: `TRAINING_`, `MISSION_`, `TEST_`
   - Include date: `_2024_12_26`

4. **Backup**: Regularly export scenarios to external location

5. **Version Control**: Include version in description
   - "v1.0 - Initial setup"
   - "v1.1 - Added second radar"

## Keyboard Shortcuts (Planned)

Future versions will support:
- `Ctrl+O` - Open/Load Scenario
- `Ctrl+S` - Save Scenario
- `Ctrl+Shift+S` - Save As
- `Ctrl+N` - New/Clear Scenario

## Troubleshooting

### "Failed to load scenario"
- **Cause**: File not found or corrupted JSON
- **Solution**: Check file path, try another scenario

### "Failed to save scenario"
- **Cause**: Invalid filename or permission denied
- **Solution**: Check filename for special characters, verify write permissions

### "No saved scenarios found"
- **Cause**: First time use, empty directory
- **Solution**: Use sample scenarios or create your first scenario

### Scenario loads but objects missing
- **Cause**: Incomplete scenario data
- **Solution**: Verify scenario file contains objects array

## Support Files

Detailed documentation:
- `SCENARIO_DROPDOWN_FEATURE.md` - Complete implementation details
- `SCENARIO_FEATURE_README.md` - Original feature specification
- `SCENARIO_IMPLEMENTATION_SUMMARY.md` - Implementation summary
- `QUICK_START_SCENARIO.md` - Getting started guide

## Code References

For developers who need to extend functionality:

### Load Scenario
```cpp
// In cvistarplanner.cpp, line ~104
void CVistarPlanner::on_pushButton_ScenarioMenu_clicked()
```

### Save Scenario Data
```cpp
// In cscenariomanager.cpp, line ~41
bool CScenarioManager::saveScenario(const Scenario &scenario, const QString &filePath)
```

### Load Scenario Data
```cpp
// In cscenariomanager.cpp, line ~65
bool CScenarioManager::loadScenario(const QString &filePath, Scenario &scenario)
```

### Apply Scenario to Map
```cpp
// In cvistarplanner.cpp, line ~176
void CVistarPlanner::loadScenarioToMap(const Scenario &scenario)
```

## Version History

- **v1.0** (2024-12-26)
  - Initial implementation
  - Dropdown button added
  - Load/Save functionality
  - Three sample scenarios
  - Dialog-based interface

---

**Need more help?** See `SCENARIO_DROPDOWN_FEATURE.md` for complete documentation.
