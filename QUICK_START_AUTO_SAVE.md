# Quick Start: Auto-Save Feature

## TL;DR
Your vistar routes and objects are now **automatically saved** and **automatically loaded** when you start the application!

## Key Features

### ✅ Auto-Save Triggers
- Adding objects (DRONE, FIGHTER, UAV, RADAR, LAUNCHER, etc.)
- Completing routes
- Deleting objects/routes
- Updating positions
- Modifying route waypoints

### ✅ Auto-Load
- Runs automatically on application startup
- Restores last session within 0.5 seconds

### 🎮 UI Controls

#### Save Scenario Button (Orange)
- **Location**: Top-left, next to Import Map
- **Function**: Save current scenario to custom location
- **Shortcut**: None (click to use)

#### Load Scenario Button (Cyan)
- **Location**: Below Save Scenario button
- **Function**: Load scenario from file
- **Shortcut**: None (click to use)

## Usage

### Normal Workflow (Auto-Save)
1. Start application → Previous scenario loads automatically
2. Add/modify objects and routes
3. Changes are saved automatically
4. Close application
5. Next time you start → Everything is restored!

### Manual Save
1. Click **"Save Scenario"** (orange button)
2. Choose filename and location
3. Click Save
4. Status bar shows "Scenario saved successfully!"

### Manual Load
1. Click **"Load Scenario"** (cyan button)
2. Select a `.json` scenario file
3. Click Open
4. Current objects are cleared
5. Selected scenario is loaded
6. Status bar shows "Scenario loaded successfully!"

## File Location

**Auto-save file**: 
- Windows: `%APPDATA%/VistarPlanner/scenarios/current_scenario.json`
- Linux: `~/.local/share/VistarPlanner/scenarios/current_scenario.json`

## What's Saved?

✅ **Yes**:
- All objects (position, type, altitude)
- All routes (waypoints, altitudes)
- Object IDs

❌ **No**:
- Map zoom/position
- UI state
- Child missiles (auto-regenerated)

## Tips

1. **Backup**: Use manual save to create backups of important scenarios
2. **Naming**: Use descriptive names like `mission_alpha_v1.json`
3. **Sharing**: Share `.json` files with team members
4. **Testing**: Create test scenarios and load them quickly

## Example Workflow

```
Day 1:
- Create 5 radars
- Add 3 UAV routes
- Close app (auto-saved)

Day 2:
- Start app → All 5 radars and 3 routes appear!
- Add 2 fighters
- Modify UAV positions
- Manual save as "mission_bravo.json" (backup)
- Continue working (auto-save active)

Day 3:
- Start app → Yesterday's work restored
- Click "Load Scenario"
- Select "mission_bravo.json"
- Back to Day 2 state!
```

## Status Messages

- ✅ **"Scenario saved successfully!"** - Save worked
- ✅ **"Scenario loaded successfully!"** - Load worked
- ❌ **"Failed to save scenario!"** - Check file permissions
- ❌ **"Failed to load scenario!"** - Check file exists/is valid JSON

## Troubleshooting

**Problem**: Auto-load not working
**Solution**: Check console for errors, verify file exists

**Problem**: Can't save manually
**Solution**: Check write permissions in target folder

**Problem**: Objects missing after load
**Solution**: Child missiles are regenerated automatically, check JSON file is valid

---

**Need more info?** See `SCENARIO_AUTO_SAVE_GUIDE.md` for complete documentation.
