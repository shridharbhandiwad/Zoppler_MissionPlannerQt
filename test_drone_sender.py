#!/usr/bin/env python3
"""
Test script to send drone data to the VistarPlanner application.
This script simulates drone data being sent via UDP multicast.
"""

import socket
import json
import time
import math

# UDP Configuration
MULTICAST_GROUP = '225.0.0.1'
MULTICAST_PORT = 8888

def send_json_data(data):
    """Send JSON data via UDP multicast"""
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)
    
    message = json.dumps(data).encode('utf-8')
    sock.sendto(message, (MULTICAST_GROUP, MULTICAST_PORT))
    sock.close()
    print(f"Sent: {json.dumps(data)}")

def test_single_drone():
    """Test sending a single drone update"""
    drone_data = {
        "STREAM": "Object",
        "TYPE": "Update",
        "ID": "DRONE_001",
        "CLASS": 1,  # VISTAR_CLASS_DRONE
        "LAT": 28.6139,  # Bangalore latitude
        "LON": 77.2090,  # Bangalore longitude
        "ALT": 100.0,
        "HEADING": 45.0,
        "VELOCITY": 25.5
    }
    send_json_data(drone_data)

def test_multiple_drones():
    """Test sending updates for multiple drones"""
    base_lat = 28.6139
    base_lon = 77.2090
    
    for i in range(3):
        drone_data = {
            "STREAM": "Object",
            "TYPE": "Update",
            "ID": f"DRONE_{i+1:03d}",
            "CLASS": 1,  # VISTAR_CLASS_DRONE
            "LAT": base_lat + (i * 0.01),
            "LON": base_lon + (i * 0.01),
            "ALT": 100.0 + (i * 20),
            "HEADING": 45.0 + (i * 30),
            "VELOCITY": 25.5 + (i * 5)
        }
        send_json_data(drone_data)
        time.sleep(0.1)

def test_moving_drone():
    """Test sending continuous updates for a moving drone"""
    drone_id = "DRONE_MOVING"
    base_lat = 28.6139
    base_lon = 77.2090
    
    print(f"\nSimulating moving drone '{drone_id}' for 30 seconds...")
    print("Press Ctrl+C to stop\n")
    
    try:
        t = 0
        while True:
            # Calculate circular movement
            radius = 0.01  # degrees
            lat = base_lat + radius * math.sin(t)
            lon = base_lon + radius * math.cos(t)
            heading = (t * 180 / math.pi) % 360
            
            drone_data = {
                "STREAM": "Object",
                "TYPE": "Update",
                "ID": drone_id,
                "CLASS": 1,  # VISTAR_CLASS_DRONE
                "LAT": lat,
                "LON": lon,
                "ALT": 150.0,
                "HEADING": heading,
                "VELOCITY": 30.0
            }
            
            send_json_data(drone_data)
            time.sleep(1.0)
            t += 0.2
            
    except KeyboardInterrupt:
        print("\nStopped simulation")

def test_various_objects():
    """Test different object types"""
    objects = [
        {"ID": "DRONE_001", "CLASS": 1, "LAT": 28.6139, "LON": 77.2090, "ALT": 100},
        {"ID": "FIGHTER_001", "CLASS": 3, "LAT": 28.6239, "LON": 77.2190, "ALT": 5000},
        {"ID": "UAV_001", "CLASS": 4, "LAT": 28.6339, "LON": 77.2290, "ALT": 200},
        {"ID": "RADAR_001", "CLASS": 5, "LAT": 28.6439, "LON": 77.2390, "ALT": 0},
    ]
    
    for obj in objects:
        obj_data = {
            "STREAM": "Object",
            "TYPE": "Update",
            "ID": obj["ID"],
            "CLASS": obj["CLASS"],
            "LAT": obj["LAT"],
            "LON": obj["LON"],
            "ALT": obj["ALT"],
            "HEADING": 0.0,
            "VELOCITY": 0.0
        }
        send_json_data(obj_data)
        time.sleep(0.2)

def test_route():
    """Test sending route data"""
    route_data = {
        "STREAM": "Route",
        "TYPE": "Update",
        "ID": "ROUTE_001",
        "WAYPOINTS": [
            {"LAT": 28.6139, "LON": 77.2090, "ALT": 100},
            {"LAT": 28.6239, "LON": 77.2190, "ALT": 150},
            {"LAT": 28.6339, "LON": 77.2290, "ALT": 200},
            {"LAT": 28.6439, "LON": 77.2390, "ALT": 150},
        ],
        "COLOR": "#00FFFF"
    }
    send_json_data(route_data)

def send_start_event():
    """Send start event"""
    event_data = {
        "STREAM": "Event",
        "TYPE": "Start"
    }
    send_json_data(event_data)

def send_stop_event():
    """Send stop event"""
    event_data = {
        "STREAM": "Event",
        "TYPE": "Stop"
    }
    send_json_data(event_data)

def main():
    print("=== VistarPlanner Drone Data Test Sender ===\n")
    print("Select test mode:")
    print("1. Single drone update")
    print("2. Multiple drones (3 drones)")
    print("3. Moving drone (continuous updates)")
    print("4. Various object types")
    print("5. Route test")
    print("6. Send Start event")
    print("7. Send Stop event")
    print("8. Full test sequence")
    
    choice = input("\nEnter choice (1-8): ").strip()
    
    if choice == '1':
        test_single_drone()
    elif choice == '2':
        test_multiple_drones()
    elif choice == '3':
        test_moving_drone()
    elif choice == '4':
        test_various_objects()
    elif choice == '5':
        test_route()
    elif choice == '6':
        send_start_event()
    elif choice == '7':
        send_stop_event()
    elif choice == '8':
        print("\nRunning full test sequence...")
        send_start_event()
        time.sleep(1)
        test_various_objects()
        time.sleep(1)
        test_route()
        time.sleep(1)
        print("\nTest sequence complete!")
    else:
        print("Invalid choice")

if __name__ == "__main__":
    main()
