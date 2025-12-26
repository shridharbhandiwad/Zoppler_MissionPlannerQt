# Scenario Management - Quick Start Guide

## 🚀 Get Started in 60 Seconds

### 1️⃣ Find the Scenario Button
Look for the **orange "Scenario" button** in the top-left of the map (next to "Import Map")

### 2️⃣ Load a Sample Scenario
```
Click "Scenario" → "Load Scenario" → "Sample Scenarios" tab
→ Double-click "Basic Defense Setup"
```

### 3️⃣ Save Your Own Scenario
```
Set up your objects and routes
→ Click "Scenario" → "Save Scenario"
→ Enter a name → Click "Save"
```

---

## 📋 What You Can Do

| Action | How To |
|--------|--------|
| **Load Sample** | Scenario → Load Scenario → Sample Scenarios tab |
| **Load Saved** | Scenario → Load Scenario → Saved Scenarios tab |
| **Load Custom** | Use "Browse for File..." button in Load dialog |
| **Save Current** | Scenario → Save Scenario → Enter name → Save |

---

## 🎯 Sample Scenarios Included

### 1. Basic Defense Setup (Easy)
- 1 Radar + 1 Launcher + 1 Fighter
- 1 Patrol route
- Perfect for learning

### 2. Air Patrol Mission (Medium)
- 2 UAVs + 1 Drone Swarm
- 2 Patrol routes
- Surveillance focus

### 3. Multi-Target Engagement (Hard)
- 2 Radars + 2 Launchers + 2 Fighters
- 2 Complex routes
- Full combat scenario

---

## 📁 Where Are Scenarios Stored?

### Default Locations
- **Windows**: `C:\Users\<you>\AppData\Local\VistarPlanner\scenarios\`
- **Linux**: `~/.local/share/VistarPlanner/scenarios/`
- **macOS**: `~/Library/Application Support/VistarPlanner/scenarios/`

### Sample Templates
Located in: `<project>/sample_scenarios/*.json`

---

## 💡 Pro Tips

### Loading
- **Double-click** any scenario to load it instantly
- **Hover** over sample scenarios to see descriptions
- Use **Browse** to load scenarios from anywhere

### Saving
- Scenario names can't contain: `< > : " / \ | ? *`
- Add a description to remember what the scenario is for
- The app warns you before overwriting existing scenarios

### Organization
- Use descriptive names: `Training_Day1`, `Mission_Alpha`
- Keep related scenarios in the same directory
- Export scenarios as `.json` files to share with team

---

## 🎨 JSON Format (For Power Users)

Scenarios are just JSON files you can edit:

```json
{
    "name": "My Scenario",
    "description": "What this scenario does",
    "objects": [
        {
            "id": "DRONE_001",
            "type": "DRONE",
            "latitude": 28.5,
            "longitude": 77.2,
            "altitude": 1000.0
        }
    ],
    "routes": [
        {
            "id": "ROUTE_001",
            "name": "Patrol Alpha",
            "waypoints": [...]
        }
    ]
}
```

---

## ❓ Troubleshooting

### Scenario Button Not Visible?
- Rebuild the application with updated sources
- Check that the main window has loaded completely

### Can't Load Scenario?
- Verify the JSON file is valid
- Check file permissions
- Make sure all required fields are present

### Can't Save Scenario?
- Ensure directory exists and is writable
- Remove invalid characters from name
- Check available disk space

---

## 📖 Need More Help?

| Document | What It Covers |
|----------|----------------|
| **SCENARIO_FEATURE_README.md** | Complete user guide |
| **SCENARIO_IMPLEMENTATION_SUMMARY.md** | Developer documentation |
| **UI_VISUAL_GUIDE.md** | UI mockups and design |
| **CHANGELOG_SCENARIO_FEATURE.md** | All changes made |

---

## 🎯 Common Workflows

### Training New Operators
1. Load "Basic Defense Setup" sample
2. Explain each object's purpose
3. Show patrol route
4. Let them modify and save as "Training_<Name>"

### Creating Custom Missions
1. Place objects on map using existing tools
2. Define routes
3. Click Scenario → Save Scenario
4. Name it (e.g., "Mission_2025_01")
5. Add description of objectives
6. Share the `.json` file with team

### Testing Different Configurations
1. Load base scenario
2. Modify one aspect
3. Save with descriptive name
4. Compare results
5. Keep best configuration

### Preparing Demonstrations
1. Create scenario ahead of time
2. Save with clear name
3. Load during presentation
4. Everything appears instantly

---

## ⌨️ Keyboard Tips (Coming Soon)

Future version may include:
- `Ctrl+L`: Load scenario
- `Ctrl+S`: Save scenario
- `Enter`: Confirm dialog
- `Esc`: Cancel dialog

---

## 🔧 Build Instructions

### Quick Build
```bash
cd /workspace
qmake VistarPlanner.pro
make
```

### Clean Build
```bash
make clean
qmake VistarPlanner.pro
make
```

---

## 📊 Feature Summary

| Feature | Status |
|---------|--------|
| Load Scenarios | ✅ Working |
| Save Scenarios | ✅ Working |
| Sample Scenarios | ✅ 3 Included |
| JSON Format | ✅ Documented |
| File Browser | ✅ Integrated |
| Validation | ✅ Real-time |
| Error Handling | ✅ User-friendly |

---

## 🎉 You're Ready!

That's all you need to know to start using scenario management. The feature is intuitive and self-explanatory. Just click the orange "Scenario" button and explore!

**Happy Mission Planning! 🚁🛡️**

---

*For detailed information, see the complete documentation in SCENARIO_FEATURE_README.md*
