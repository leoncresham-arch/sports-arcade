"""Mock contract checks for the native capture request — not an Unreal render."""
import importlib.util
import json
from pathlib import Path
import sys
import tempfile
import types
import unittest
from unittest.mock import patch

SCRIPT = Path(__file__).resolve().parents[1] / "Tools/Editor/capture_visual_review.py"


class Actor:
    def __init__(self, label):
        self.label = label

    def get_actor_label(self):
        return self.label


class CaptureRequestTests(unittest.TestCase):
    def setUp(self):
        self.actor = Actor("SA | Broadcast Camera")
        self.requests = []
        fake = types.ModuleType("unreal")
        fake.EditorActorSubsystem = object()
        fake.get_editor_subsystem = lambda _: types.SimpleNamespace(
            get_all_level_actors=lambda: [Actor("SA | West Goal"), self.actor])
        fake.AutomationLibrary = types.SimpleNamespace(
            take_high_res_screenshot=lambda *a, **kw: self.requests.append((a, kw)) or True
        )
        fake.log = lambda _: None
        self.fake = fake

    def load(self):
        spec = importlib.util.spec_from_file_location("sports_arcade_capture_mock", SCRIPT)
        module = importlib.util.module_from_spec(spec)
        with patch.dict(sys.modules, {"unreal": self.fake}):
            spec.loader.exec_module(module)
        return module

    def test_requires_approved_asset_acknowledgement(self):
        mod = self.load()
        with tempfile.TemporaryDirectory() as folder:
            with self.assertRaisesRegex(RuntimeError, "explicitly confirms"):
                mod.capture_review(folder)
            self.assertEqual(self.requests, [])
            self.assertEqual(list(Path(folder).iterdir()), [])

    def test_requests_screenshot_and_writes_unverified_provenance(self):
        mod = self.load()
        with tempfile.TemporaryDirectory() as folder:
            record = mod.capture_review(folder, approved_assets=True)
            self.assertEqual(self.requests[0][0], (1920, 1080, "sports-arcade-unreal-review.png"))
            self.assertIs(self.requests[0][1]["camera"], self.actor)
            self.assertIn("UNVERIFIED", record["verification"])
            log = json.loads((Path(folder) / "sports-arcade-capture-request.json").read_text())
            self.assertEqual(log["camera_label"], "SA | Broadcast Camera")

    def test_rejected_screenshot_request_does_not_write_success_log(self):
        self.fake.AutomationLibrary.take_high_res_screenshot = lambda *a, **kw: False
        mod = self.load()
        with tempfile.TemporaryDirectory() as folder:
            with self.assertRaisesRegex(RuntimeError, "rejected the screenshot request"):
                mod.capture_review(folder, approved_assets=True)
            self.assertEqual(list(Path(folder).iterdir()), [])

    def test_rejects_missing_broadcast_camera(self):
        self.fake.get_editor_subsystem = lambda _: types.SimpleNamespace(
            get_all_level_actors=lambda: [Actor("Unrelated")])
        mod = self.load()
        with tempfile.TemporaryDirectory() as folder:
            with self.assertRaisesRegex(RuntimeError, "Broadcast camera not found"):
                mod.capture_review(folder, approved_assets=True)
            self.assertEqual(self.requests, [])

    def test_rejects_oversized_capture(self):
        mod = self.load()
        with tempfile.TemporaryDirectory() as folder:
            with self.assertRaises(ValueError):
                mod.capture_review(folder, width=12000, approved_assets=True)
            self.assertEqual(self.requests, [])


if __name__ == "__main__":
    unittest.main()
