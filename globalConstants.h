#ifndef GLOBALCONSTANTS_H
#define GLOBALCONSTANTS_H


#define APP_VERSION "v0.1"
#define APP_BUILD_DATE "23rd June 2025"

const double MAX_ZOOMOUT_THRESHOLD = 4.0;
const double TEXT_VISIBLE_THRESHOLD = 300.0;  // Text visible below this

#define VISTAR_SRC_MISSION_PLANNER "MISSION_PLANNER"
#define VISTAR_SRC_MISSION_EXECUTOR  "MISSION_EXECUTOR"

enum eVISTAR_CLASS {
    VISTAR_CLASS_NONE,
    VISTAR_CLASS_DRONE,
    VISTAR_CLASS_DRONE_SWARM,
    VISTAR_CLASS_FIGHTER,
    VISTAR_CLASS_UAV,
    VISTAR_CLASS_RADAR,
    VISTAR_CLASS_LAUNCHER,
    VISTAR_CLASS_MISSILE,
    VISTAR_CLASS_ROUTE
};

// Path/Trajectory generation types
enum eVISTAR_PATH_TYPE {
    PATH_TYPE_NONE = 0,
    PATH_TYPE_STRAIGHT,      // Direct line between two points
    PATH_TYPE_S_CURVE,       // Smooth S-shaped curve (sigmoid)
    PATH_TYPE_L_CURVE,       // L-shaped path with rounded corner
    PATH_TYPE_FIGURE_8,      // Figure-8 trajectory
    PATH_TYPE_SPIRAL,        // Spiral path between points
    PATH_TYPE_ZIGZAG,        // Zigzag evasive maneuver
    PATH_TYPE_BEZIER,        // Smooth Bezier curve
    PATH_TYPE_RANDOM         // Random waypoints between points
};

#endif // GLOBALCONSTANTS_H
