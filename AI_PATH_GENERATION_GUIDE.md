# AI-Enabled Path Generation Guide

## Overview

VistarPlanner now supports both **Conventional (Algorithmic)** and **AI-Powered (Intelligent)** path generation methods. Users can choose between these methods based on their mission requirements.

## Path Generation Methods

### 1. Conventional Path Generation (Algorithmic)

Traditional mathematical algorithms for generating predictable, pattern-based flight paths:

| Path Type | Description | Use Case |
|-----------|-------------|----------|
| **Straight Line** | Direct path between two points | Quick transit, direct approach |
| **S-Curve** | Smooth S-shaped trajectory using sigmoid function | Gradual heading changes |
| **L-Curve** | L-shaped path with rounded corner | Perpendicular approach maneuvers |
| **Figure-8** | Figure-8 looping trajectory | Surveillance patterns, loiter |
| **Spiral** | Expanding spiral path | Area coverage, search patterns |
| **Zigzag** | Zigzag evasive maneuver pattern | Tactical approaches, evasion |
| **Bezier Curve** | Smooth Bezier curve trajectory | Elegant curved paths |
| **Random** | Random waypoints | Unpredictable paths, testing |

### 2. AI-Powered Path Generation (Intelligent)

Mission-aware intelligent path generation with threat avoidance and optimization:

| Mission Type | Description | AI Behavior |
|--------------|-------------|-------------|
| **🔍 Patrol** | Area surveillance mission | Racetrack/oval patterns, optimal coverage |
| **💥 Strike** | Attack mission | Ingress at low altitude, pop-up attack, egress escape |
| **📡 Recon** | Reconnaissance mission | Zigzag coverage, observation points, sensor optimization |
| **🛡️ Evasion** | Avoid detection | Unpredictable path, random direction changes |
| **🎯 Intercept** | Target pursuit | Optimal climb, high-speed cruise, diving attack |
| **✈️ Escort** | Protection mission | Weaving pattern around protected asset |

## How to Use

### Accessing Path Generation

1. Click on **"Path Generator"** button in the main toolbar
2. A dropdown menu appears with options:
   - **⚙ Settings** - Configure parameters
   - **🤖 AI Path Generation** - AI mission types submenu
   - **⚙️ Conventional Paths** - Traditional algorithms submenu

### Using AI Path Generation

1. Click **"Path Generator"** → **"🤖 AI Path Generation"**
2. Select a mission type (e.g., "🔍 Patrol Mission")
3. Click on the map to set the **START** point (cyan marker appears)
4. Click again to set the **END** point
5. AI generates the intelligent mission path automatically

### Using Conventional Path Generation

1. Click **"Path Generator"** → **"⚙️ Conventional Paths"**
2. Select a path type (e.g., "〰 S-Curve")
3. Click on the map to set the **START** point (green marker appears)
4. Click again to set the **END** point
5. Path is generated using the selected algorithm

### Configuring Settings

1. Click **"Path Generator"** → **"⚙ Settings"**
2. The **Path Generator Settings** dialog opens with two sections:

#### Path Generation Method
- **⚙️ Conventional (Algorithmic)** - Traditional mathematical algorithms
- **🤖 AI-Powered (Intelligent)** - Mission-aware intelligent generation

#### AI Path Settings (when AI mode selected)
- **Mission Type** - Select the mission objective
- **Min/Max Altitude** - Altitude constraints for the path
- **Threat Radius** - Detection radius for avoidance calculations
- **Avoid Detection** - Enable threat avoidance behavior
- **Terrain Following** - Enable low-altitude terrain-following mode
- **Optimize for Fuel** - Generate more direct fuel-efficient paths

#### General Parameters
- **Number of Waypoints** - Path resolution (more = smoother)
- **Default Altitude** - Base flight altitude

#### Curve Parameters
- **Curve Factor** - Intensity of curves (0-1)
- **Spiral Turns** - Number of spiral rotations
- **Max Turn Radius** - Maximum turn radius
- **Spread Radius** - Lateral spread in km

#### Zigzag/Random Parameters
- **Zigzag Amplitude/Frequency** - Zigzag characteristics
- **Randomness Level** - Global randomness for all paths

## AI Mission Types Explained

### 🔍 Patrol Mission
- Creates racetrack/oval patrol patterns
- Optimized for area coverage and surveillance
- Includes altitude variations for sensor effectiveness
- Supports threat avoidance when enabled

### 💥 Strike Mission
- **Ingress Phase**: Low altitude approach to avoid detection
- **Attack Phase**: Pop-up maneuver for weapons release
- **Egress Phase**: High-speed escape in random direction
- Terrain-following capable when enabled

### 📡 Reconnaissance Mission
- Zigzag pattern for maximum sensor coverage
- Includes observation hold points
- High altitude for better sensor range
- Supports threat avoidance routing

### 🛡️ Evasion Mission
- Highly unpredictable random path
- Dramatic altitude changes
- Direction changes to avoid pursuit
- Minimum bias toward destination

### 🎯 Intercept Mission
- **Climb Phase**: Rapid climb to optimal altitude
- **Cruise Phase**: High-speed approach
- **Attack Phase**: Diving intercept trajectory
- Optimized for speed and energy management

### ✈️ Escort Mission
- Weaving S-pattern around main route
- Maintains proximity to protected asset
- Alert/defensive position variations
- Higher altitude for protection advantage

## Keyboard Shortcuts

- **Escape** - Cancel path generation mode
- **Home** - Return to India home view

## Tips

1. **For surveillance missions**, use AI Patrol or Recon for optimal coverage
2. **For attack missions**, use AI Strike for realistic ingress/egress planning
3. **For unpredictable paths**, use AI Evasion or Conventional Random
4. **For smooth transitions**, use Conventional Bezier or S-Curve
5. **Enable "Avoid Detection"** in AI settings when threats are present
6. **Adjust "Randomness Level"** in settings for more/less variation

## Technical Notes

- AI paths use intelligent heuristics for mission-aware generation
- Paths are automatically saved with scenarios
- All generated routes can be attached to aircraft/UAV objects
- Path parameters are persisted across sessions when saved as defaults
