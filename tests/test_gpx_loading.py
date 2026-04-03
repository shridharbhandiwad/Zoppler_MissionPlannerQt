"""
test_gpx_loading.py
-------------------
Standalone Python test that validates the GPX parsing logic expected by
CScenarioManager::loadGpxScenario() without needing the Qt/QGIS runtime.

Run:
    python3 tests/test_gpx_loading.py
"""

import xml.etree.ElementTree as ET
import os
import sys
import unittest

# ---------------------------------------------------------------------------
# Minimal Python re-implementation of the C++ GPX → Scenario conversion
# (mirrors CScenarioManager exactly so the same test data drives both).
# ---------------------------------------------------------------------------

GPX_NS = "http://www.topografix.com/GPX/1/1"


def _tag(local: str) -> str:
    return f"{{{GPX_NS}}}{local}"


def parse_gpx(filepath: str) -> dict:
    """Parse a GPX file and return a dict that mirrors the Scenario struct."""

    tree = ET.parse(filepath)
    root = tree.getroot()

    # Strip namespace for tag comparison
    assert root.tag in (
        "gpx",
        _tag("gpx"),
    ), f"Root element is not <gpx>: {root.tag}"

    scenario: dict = {
        "name": os.path.splitext(os.path.basename(filepath))[0],
        "description": "",
        "createdDate": "",
        "objects": [],
        "routes": [],
        "metadata": {"source": "gpx"},
    }

    def find(elem, local):
        # Try with and without namespace
        child = elem.find(_tag(local))
        if child is None:
            child = elem.find(local)
        return child

    def findall(elem, local):
        children = elem.findall(_tag(local))
        if not children:
            children = elem.findall(local)
        return children

    meta = find(root, "metadata")
    if meta is not None:
        n = find(meta, "name")
        if n is not None and n.text:
            scenario["name"] = n.text.strip()
        d = find(meta, "desc")
        if d is not None and d.text:
            scenario["description"] = d.text.strip()
        t = find(meta, "time")
        if t is not None and t.text:
            scenario["createdDate"] = t.text.strip()

    # Waypoints → objects
    for i, wpt in enumerate(findall(root, "wpt")):
        lat = float(wpt.get("lat", 0))
        lon = float(wpt.get("lon", 0))
        ele_elem = find(wpt, "ele")
        alt = float(ele_elem.text) if ele_elem is not None else 0.0
        name_elem = find(wpt, "name")
        wpt_name = name_elem.text.strip() if name_elem is not None else f"WPT_{i+1:03d}"
        type_elem = find(wpt, "type")
        wpt_type = type_elem.text.strip().upper() if type_elem is not None else "WAYPOINT"
        scenario["objects"].append(
            {
                "id": wpt_name,
                "type": wpt_type,
                "latitude": lat,
                "longitude": lon,
                "altitude": alt,
            }
        )

    # Tracks → routes
    for i, trk in enumerate(findall(root, "trk")):
        route = {
            "id": f"TRK_{i+1:03d}",
            "name": f"Track_{i+1}",
            "gpxType": "track",
            "waypoints": [],
        }
        name_elem = find(trk, "name")
        if name_elem is not None and name_elem.text:
            route["name"] = name_elem.text.strip()
        type_elem = find(trk, "type")
        route["type"] = type_elem.text.strip() if type_elem is not None else "track"

        for trkseg in findall(trk, "trkseg"):
            for trkpt in findall(trkseg, "trkpt"):
                lat = float(trkpt.get("lat", 0))
                lon = float(trkpt.get("lon", 0))
                ele_elem = find(trkpt, "ele")
                alt = float(ele_elem.text) if ele_elem is not None else 0.0
                route["waypoints"].append({"lat": lat, "lon": lon, "alt": alt})

        scenario["routes"].append(route)

    # <rte> routes → routes
    for i, rte in enumerate(findall(root, "rte")):
        route = {
            "id": f"RTE_{i+1:03d}",
            "name": f"Route_{i+1}",
            "gpxType": "route",
            "waypoints": [],
        }
        name_elem = find(rte, "name")
        if name_elem is not None and name_elem.text:
            route["name"] = name_elem.text.strip()
        type_elem = find(rte, "type")
        route["type"] = type_elem.text.strip() if type_elem is not None else "route"

        for rtept in findall(rte, "rtept"):
            lat = float(rtept.get("lat", 0))
            lon = float(rtept.get("lon", 0))
            ele_elem = find(rtept, "ele")
            alt = float(ele_elem.text) if ele_elem is not None else 0.0
            route["waypoints"].append({"lat": lat, "lon": lon, "alt": alt})

        scenario["routes"].append(route)

    # Mirror C++ behaviour: if the GPX has only waypoints (no tracks/routes),
    # synthesise a connecting route so the sequence is visible on the canvas.
    if scenario["objects"] and not scenario["routes"]:
        synthetic = {
            "id": "WPT_ROUTE_001",
            "name": scenario["name"] + " (waypoint path)",
            "gpxType": "waypoints",
            "type": "waypoint_path",
            "waypoints": [
                {"lat": o["latitude"], "lon": o["longitude"], "alt": o["altitude"]}
                for o in scenario["objects"]
            ],
        }
        scenario["routes"].append(synthetic)

    return scenario


# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------

SAMPLE_GPX = os.path.join(
    os.path.dirname(__file__), "..", "sample_scenarios", "Sample_GPX_Mission.gpx"
)


class TestGpxParsing(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        assert os.path.exists(SAMPLE_GPX), f"Sample GPX not found: {SAMPLE_GPX}"
        cls.scenario = parse_gpx(SAMPLE_GPX)

    # -- Metadata --

    def test_scenario_name(self):
        self.assertEqual(self.scenario["name"], "Sample GPX Mission")

    def test_scenario_description(self):
        self.assertIn("VistarPlanner", self.scenario["description"])

    def test_created_date(self):
        self.assertTrue(self.scenario["createdDate"].startswith("2026"))

    def test_source_metadata(self):
        self.assertEqual(self.scenario["metadata"]["source"], "gpx")

    # -- Waypoints (objects) --

    def test_waypoint_count(self):
        self.assertEqual(len(self.scenario["objects"]), 3)

    def test_radar_object(self):
        radar = next(o for o in self.scenario["objects"] if o["id"] == "RADAR_001")
        self.assertEqual(radar["type"], "RADAR")
        self.assertAlmostEqual(radar["latitude"], 28.5000, places=4)
        self.assertAlmostEqual(radar["longitude"], 77.2000, places=4)
        self.assertAlmostEqual(radar["altitude"], 100.0, places=1)

    def test_launcher_object(self):
        launcher = next(o for o in self.scenario["objects"] if o["id"] == "LAUNCHER_001")
        self.assertEqual(launcher["type"], "LAUNCHER")
        self.assertAlmostEqual(launcher["latitude"], 28.5100, places=4)
        self.assertAlmostEqual(launcher["longitude"], 77.2100, places=4)
        self.assertAlmostEqual(launcher["altitude"], 50.0, places=1)

    def test_fighter_object(self):
        fighter = next(o for o in self.scenario["objects"] if o["id"] == "FIGHTER_001")
        self.assertEqual(fighter["type"], "FIGHTER")
        self.assertAlmostEqual(fighter["altitude"], 5000.0, places=1)

    # -- Routes --

    def test_route_count(self):
        # 1 <rte> + 1 <trk>
        self.assertEqual(len(self.scenario["routes"]), 2)

    def test_track_route(self):
        trk = next(r for r in self.scenario["routes"] if r["gpxType"] == "track")
        self.assertEqual(trk["name"], "CAP Route Alpha")
        self.assertEqual(trk["type"], "patrol")
        self.assertEqual(len(trk["waypoints"]), 5)
        first = trk["waypoints"][0]
        self.assertAlmostEqual(first["lat"], 28.4500, places=4)
        self.assertAlmostEqual(first["lon"], 77.1500, places=4)
        self.assertAlmostEqual(first["alt"], 5000.0, places=1)

    def test_rte_route(self):
        rte = next(r for r in self.scenario["routes"] if r["gpxType"] == "route")
        self.assertEqual(rte["name"], "Intercept Vector")
        self.assertEqual(rte["type"], "intercept")
        self.assertEqual(len(rte["waypoints"]), 3)
        last = rte["waypoints"][-1]
        self.assertAlmostEqual(last["lat"], 28.6500, places=4)
        self.assertAlmostEqual(last["lon"], 77.3500, places=4)
        self.assertAlmostEqual(last["alt"], 12000.0, places=1)

    def test_track_route_id_format(self):
        trk = next(r for r in self.scenario["routes"] if r["gpxType"] == "track")
        self.assertTrue(trk["id"].startswith("TRK_"))

    def test_rte_route_id_format(self):
        rte = next(r for r in self.scenario["routes"] if r["gpxType"] == "route")
        self.assertTrue(rte["id"].startswith("RTE_"))

    # -- Edge cases --

    def test_invalid_file_raises(self):
        """parse_gpx should raise an exception for a non-XML file."""
        import tempfile

        with tempfile.NamedTemporaryFile(mode="w", suffix=".gpx", delete=False) as f:
            f.write("this is not xml")
            tmp_path = f.name
        try:
            with self.assertRaises(ET.ParseError):
                parse_gpx(tmp_path)
        finally:
            os.unlink(tmp_path)

    def test_gpx_with_no_waypoints(self):
        """A GPX with only a track and no waypoints must still load cleanly."""
        import tempfile

        gpx_content = """<?xml version="1.0" encoding="UTF-8"?>
<gpx version="1.1" xmlns="http://www.topografix.com/GPX/1/1">
  <metadata><name>TrackOnly</name></metadata>
  <trk>
    <name>Solo Track</name>
    <trkseg>
      <trkpt lat="10.0" lon="20.0"><ele>500.0</ele></trkpt>
      <trkpt lat="10.1" lon="20.1"><ele>510.0</ele></trkpt>
    </trkseg>
  </trk>
</gpx>"""
        with tempfile.NamedTemporaryFile(mode="w", suffix=".gpx", delete=False) as f:
            f.write(gpx_content)
            tmp_path = f.name
        try:
            scenario = parse_gpx(tmp_path)
            self.assertEqual(scenario["name"], "TrackOnly")
            self.assertEqual(len(scenario["objects"]), 0)
            self.assertEqual(len(scenario["routes"]), 1)
            self.assertEqual(len(scenario["routes"][0]["waypoints"]), 2)
        finally:
            os.unlink(tmp_path)

    def test_gpx_with_no_elevation(self):
        """Track points without <ele> should default altitude to 0.0."""
        import tempfile

        gpx_content = """<?xml version="1.0" encoding="UTF-8"?>
<gpx version="1.1" xmlns="http://www.topografix.com/GPX/1/1">
  <trk>
    <name>No Ele Track</name>
    <trkseg>
      <trkpt lat="5.0" lon="6.0"/>
    </trkseg>
  </trk>
</gpx>"""
        with tempfile.NamedTemporaryFile(mode="w", suffix=".gpx", delete=False) as f:
            f.write(gpx_content)
            tmp_path = f.name
        try:
            scenario = parse_gpx(tmp_path)
            self.assertAlmostEqual(scenario["routes"][0]["waypoints"][0]["alt"], 0.0)
        finally:
            os.unlink(tmp_path)


BANGALORE_GPX = os.path.join(
    os.path.dirname(__file__), "..", "sample_scenarios", "Bangalore_Waypoints.gpx"
)


class TestBangaloreWaypointsGpx(unittest.TestCase):
    """
    Tests against the 25-waypoint Bangalore GPX that the user reported as 'not plotted'.
    Root cause was: <wpt> elements with no <type> child defaulted to "WAYPOINT" which
    was not a recognised class in CMapCanvas::getEnumFromClassName → objects were skipped.
    Fix: VISTAR_CLASS_WAYPOINT is now a first-class citizen.
    """

    @classmethod
    def setUpClass(cls):
        assert os.path.exists(BANGALORE_GPX), f"Bangalore GPX not found: {BANGALORE_GPX}"
        cls.scenario = parse_gpx(BANGALORE_GPX)

    def test_name(self):
        self.assertEqual(self.scenario["name"], "Bangalore Waypoints")

    def test_waypoint_count(self):
        self.assertEqual(len(self.scenario["objects"]), 25)

    def test_all_waypoints_have_waypoint_type(self):
        """All wpts without a <type> child must default to WAYPOINT (not empty / unknown)."""
        for obj in self.scenario["objects"]:
            self.assertEqual(
                obj["type"],
                "WAYPOINT",
                f"{obj['id']} has unexpected type {obj['type']!r}",
            )

    def test_waypoint_ids_are_named(self):
        ids = [o["id"] for o in self.scenario["objects"]]
        self.assertIn("WP1", ids)
        self.assertIn("WP25", ids)

    def test_first_waypoint_coordinates(self):
        wp1 = next(o for o in self.scenario["objects"] if o["id"] == "WP1")
        self.assertAlmostEqual(wp1["latitude"], 12.976380, places=5)
        self.assertAlmostEqual(wp1["longitude"], 77.594032, places=5)

    def test_last_waypoint_coordinates(self):
        wp25 = next(o for o in self.scenario["objects"] if o["id"] == "WP25")
        self.assertAlmostEqual(wp25["latitude"], 12.969014, places=5)
        self.assertAlmostEqual(wp25["longitude"], 77.597456, places=5)

    def test_synthetic_route_created(self):
        """Waypoint-only GPX must produce exactly one auto-synthesised route."""
        self.assertEqual(len(self.scenario["routes"]), 1)

    def test_synthetic_route_id(self):
        self.assertEqual(self.scenario["routes"][0]["id"], "WPT_ROUTE_001")

    def test_synthetic_route_gpx_type(self):
        self.assertEqual(self.scenario["routes"][0]["gpxType"], "waypoints")

    def test_synthetic_route_waypoint_count(self):
        """Synthetic route must contain all 25 waypoints in order."""
        self.assertEqual(len(self.scenario["routes"][0]["waypoints"]), 25)

    def test_synthetic_route_first_point(self):
        first = self.scenario["routes"][0]["waypoints"][0]
        self.assertAlmostEqual(first["lat"], 12.976380, places=5)
        self.assertAlmostEqual(first["lon"], 77.594032, places=5)

    def test_synthetic_route_last_point(self):
        last = self.scenario["routes"][0]["waypoints"][-1]
        self.assertAlmostEqual(last["lat"], 12.969014, places=5)
        self.assertAlmostEqual(last["lon"], 77.597456, places=5)

    def test_all_waypoint_altitudes_default_zero(self):
        """No <ele> present in any wpt → all altitudes must be 0.0."""
        for obj in self.scenario["objects"]:
            self.assertAlmostEqual(obj["altitude"], 0.0)


class TestWaypointTypeDefaulting(unittest.TestCase):
    """Unit tests for the WAYPOINT type default and related class handling."""

    def _make_gpx(self, wpt_xml: str) -> str:
        return (
            '<?xml version="1.0" encoding="UTF-8"?>'
            '<gpx version="1.1" xmlns="http://www.topografix.com/GPX/1/1">'
            + wpt_xml
            + "</gpx>"
        )

    def _parse_inline(self, wpt_xml: str) -> dict:
        import tempfile

        content = self._make_gpx(wpt_xml)
        with tempfile.NamedTemporaryFile(mode="w", suffix=".gpx", delete=False) as f:
            f.write(content)
            tmp_path = f.name
        try:
            return parse_gpx(tmp_path)
        finally:
            os.unlink(tmp_path)

    def test_wpt_without_type_defaults_to_waypoint(self):
        scenario = self._parse_inline('<wpt lat="1.0" lon="2.0"><name>A</name></wpt>')
        self.assertEqual(scenario["objects"][0]["type"], "WAYPOINT")

    def test_wpt_with_explicit_radar_type(self):
        scenario = self._parse_inline(
            '<wpt lat="1.0" lon="2.0"><name>R1</name><type>RADAR</type></wpt>'
        )
        self.assertEqual(scenario["objects"][0]["type"], "RADAR")

    def test_wpt_type_uppercased(self):
        scenario = self._parse_inline(
            '<wpt lat="1.0" lon="2.0"><name>F1</name><type>fighter</type></wpt>'
        )
        self.assertEqual(scenario["objects"][0]["type"], "FIGHTER")

    def test_single_wpt_only_generates_synthetic_route(self):
        scenario = self._parse_inline('<wpt lat="5.0" lon="6.0"><name>X</name></wpt>')
        self.assertEqual(len(scenario["routes"]), 1)
        self.assertEqual(scenario["routes"][0]["gpxType"], "waypoints")

    def test_wpt_plus_track_does_not_generate_synthetic_route(self):
        """When a track is present, no synthetic route should be added for the wpts."""
        content = self._make_gpx(
            '<wpt lat="1.0" lon="2.0"><name>W1</name></wpt>'
            "<trk><name>T</name><trkseg>"
            '<trkpt lat="3.0" lon="4.0"/>'
            "</trkseg></trk>"
        )
        import tempfile

        with tempfile.NamedTemporaryFile(mode="w", suffix=".gpx", delete=False) as f:
            f.write(content)
            tmp_path = f.name
        try:
            scenario = parse_gpx(tmp_path)
            # Exactly 1 route (the track); no synthetic route
            self.assertEqual(len(scenario["routes"]), 1)
            self.assertEqual(scenario["routes"][0]["gpxType"], "track")
        finally:
            os.unlink(tmp_path)


if __name__ == "__main__":
    print(f"Running GPX loading tests\n")
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    suite.addTests(loader.loadTestsFromTestCase(TestGpxParsing))
    suite.addTests(loader.loadTestsFromTestCase(TestBangaloreWaypointsGpx))
    suite.addTests(loader.loadTestsFromTestCase(TestWaypointTypeDefaulting))
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    sys.exit(0 if result.wasSuccessful() else 1)
