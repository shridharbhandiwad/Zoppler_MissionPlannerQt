# Scenario Manager Tab Implementation

## Summary
This document describes the implementation of the Scenario Manager tab that groups Load Scenario, Save Scenario, and Reset Scenario functions in the VistarPlanner application.

## Changes Made

### 1. User Interface Changes (`cvistarplanner.ui`)
- **Created a new QTabWidget** named `tabWidget_ScenarioManager` at position (10, 10) with size 180x200
- **Grouped three buttons** under the "Scenario Manager" tab:
  - **Load Scenario** - Loads a scenario from a JSON file
  - **Save Scenario** - Saves the current scenario to a JSON file
  - **Reset Scenario** - NEW! Clears all objects and routes from the map canvas
- **Moved the Import Map button** to the right (position 200, 10) to make space for the tab widget
- **Applied modern styling** with blue gradient theme for the tab widget and consistent button styles

### 2. CMapCanvas Class Updates

#### Header File (`MapDisplay/cmapcanvas.h`)
Added new public method:
```cpp
void resetScenario();
```

#### Implementation File (`MapDisplay/cmapcanvas.cpp`)
Added `resetScenario()` method (lines 1276-1296):
- Clears all existing routes from `_m_listVistarRoutes`
- Clears all existing objects from `_m_listVistarObjects`
- Clears object IDs from `_m_listVistarObjectIds`
- Properly deallocates memory for all objects
- Refreshes the canvas to show the cleared state
- Logs debug messages for troubleshooting

### 3. CVistarPlanner Class Updates

#### Header File (`cvistarplanner.h`)
Added new private slot:
```cpp
void on_pushButton_ResetScenario_clicked();
```

#### Implementation File (`cvistarplanner.cpp`)
Added slot implementation (lines 136-140):
- Calls `ui->mapCanvas->resetScenario()`
- Displays status message: "Scenario reset - all objects cleared!"

## Functionality

### Load Scenario
- Opens file dialog to select a JSON scenario file
- Loads all objects and routes from the file
- Displays success/failure message in status bar

### Save Scenario
- Opens file dialog to specify save location
- Saves current scenario to JSON file
- Displays success/failure message in status bar

### Reset Scenario (NEW)
- Clears all objects and routes from the map canvas
- Properly deallocates memory to prevent memory leaks
- Refreshes the canvas display
- Displays confirmation message in status bar

## UI Layout

The Scenario Manager tab appears in the top-left corner of the map canvas with:
- **Tab Header**: Blue gradient theme with "Scenario Manager" title
- **Button Layout**: Three vertically stacked buttons
  - Load (Cyan/Teal gradient)
  - Save (Orange gradient)
  - Reset (Red gradient)
- **Dimensions**: 180px wide, 200px tall
- **Position**: (10, 10) from top-left of map canvas

## Technical Details

### Memory Management
The `resetScenario()` method properly handles memory deallocation:
1. Iterates through all routes and deletes each one
2. Clears the routes hash table
3. Iterates through all objects and deletes each one
4. Clears the objects hash table and ID list
5. Calls `refresh()` to update the display

### Qt Signal-Slot Connection
The button click is automatically connected through Qt's naming convention:
- Button name: `pushButton_ResetScenario`
- Slot name: `on_pushButton_ResetScenario_clicked()`
- Qt's meta-object system automatically connects them

## Testing Notes

The implementation follows Qt best practices and the existing codebase patterns:
- Memory management matches the pattern used in `loadScenarioToCanvas()`
- Slot naming follows Qt auto-connect conventions
- Status bar messages match the style of existing scenario operations
- UI styling is consistent with other buttons in the application

**Note**: This project requires Windows with Qt 5.15.2 and MSVC2019 to build and test. The code changes are syntactically correct and follow the existing patterns in the codebase.

## Usage

1. **To Load a Scenario**:
   - Click "Scenario Manager" tab
   - Click "Load Scenario" button
   - Select a JSON file
   - Scenario objects will appear on the map

2. **To Save a Scenario**:
   - Add objects to the map
   - Click "Scenario Manager" tab
   - Click "Save Scenario" button
   - Choose save location and filename

3. **To Reset/Clear the Map**:
   - Click "Scenario Manager" tab
   - Click "Reset Scenario" button
   - All objects and routes will be removed from the map

## Files Modified

1. `/workspace/cvistarplanner.ui` - UI layout and tab widget creation
2. `/workspace/cvistarplanner.h` - Added Reset button slot declaration
3. `/workspace/cvistarplanner.cpp` - Implemented Reset button slot
4. `/workspace/MapDisplay/cmapcanvas.h` - Added resetScenario() method declaration
5. `/workspace/MapDisplay/cmapcanvas.cpp` - Implemented resetScenario() method

## Future Enhancements

Potential improvements:
- Add confirmation dialog before reset operation
- Add undo functionality for reset operation
- Add keyboard shortcuts (e.g., Ctrl+L, Ctrl+S, Ctrl+R)
- Add recent scenarios dropdown menu
- Add scenario preview before loading
