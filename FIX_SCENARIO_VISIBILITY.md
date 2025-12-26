# Scenario Visibility Fix

## Problem
Loaded scenarios were not visible on the map canvas after loading them from the Load Scenario dialog.

## Root Cause
In the `CVistarPlanner::loadScenarioToCanvas()` function in `cvistarplanner.cpp`, the critical line that displays loaded objects on the map canvas was commented out:

```cpp
// ui->mapCanvas->slotUpdateObject(doc);  // <-- This was commented out
```

## Solution
Uncommented the line at line 247 in `cvistarplanner.cpp`:

```cpp
ui->mapCanvas->slotUpdateObject(doc);  // Now active
```

## What This Fixes
- **Objects** from loaded scenarios (RADAR, LAUNCHER, FIGHTER, UAV, DRONE_SWARM, etc.) will now properly display on the map canvas
- Sample scenarios will show their objects correctly
- Saved scenarios loaded from files will show their objects correctly

## Note About Routes
Route display functionality (line 271) remains commented out pending implementation of the `slotLoadRoute()` method in the map canvas class. This can be addressed in a future update once the appropriate method is available.

## Files Modified
- `cvistarplanner.cpp` - line 247: Uncommented the object display call

## Testing
After recompiling the application, you should be able to:
1. Open the Scenario menu
2. Select "Load Scenario"
3. Choose either a sample scenario or a saved scenario
4. See the objects displayed on the map canvas

The status bar will also show a message indicating how many objects and routes were loaded.
