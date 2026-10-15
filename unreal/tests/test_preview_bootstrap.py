"""Simulated-editor smoke tests for scene bootstrap, not a native Unreal test.

Fake APIs exercise our orchestration, geometry coordinates and overwrite safety.
Unreal APIs, C++ compilation, lighting and rendering remain unverified.
"""
import importlib.util
from pathlib import Path
import sys
import types
import unittest
from unittest.mock import patch

SCRIPT = Path(__file__).resolve().parents[1] / "Tools/Editor/setup_visual_review.py"


class Vector:
    def __init__(self, x=0.0, y=0.0, z=0.0):
        self.x, self.y, self.z = x, y, z


class Rotator:
    def __init__(self, pitch=0, yaw=0, roll=0):
        self.pitch, self.yaw, self.roll = pitch, yaw, roll


class Actor:
    def __init__(self, cls, location):
        self.cls = cls
        self.location = location
        self.label = ""
        self.rotation = None

    def set_actor_label(self, label):
        self.label = label

    def get_actor_location(self):
        return self.location

    def set_actor_rotation(self, rotation, teleport):
        self.rotation = rotation


class FakeEditor:
    def __init__(self):
        self.actors = []

    def spawn_actor_from_class(self, cls, location, rotation):
        actor = Actor(cls, location)
        self.actors.append(actor)
        return actor


class FakeAssets:
    def __init__(self, exists=False):
        self.exists = exists
        self.made = []

    def does_asset_exist(self, _):
        return self.exists

    def make_directory(self, path):
        self.made.append(path)


class FakeLevel:
    def __init__(self):
        self.created = []
        self.saved = 0

    def new_level(self, path):
        self.created.append(path)
        return True

    def save_current_level(self):
        self.saved += 1
        return True


class PreviewBootstrapTests(unittest.TestCase):
    def setUp(self):
        self.editor = FakeEditor()
        self.assets = FakeAssets()
        self.level = FakeLevel()
        editor = self.editor
        self.fake = types.ModuleType("unreal")
        self.fake.Vector = Vector
        self.fake.Rotator = Rotator
        self.fake.EditorActorSubsystem = object()
        self.fake.LevelEditorSubsystem = object()
        self.fake.get_editor_subsystem = lambda cls: (
            editor if cls is self.fake.EditorActorSubsystem
            else self.level if cls is self.fake.LevelEditorSubsystem
            else None
        )
        self.fake.EditorAssetLibrary = self.assets
        self.fake.MathLibrary = types.SimpleNamespace(find_look_at_rotation=lambda a, b: Rotator(-30, 0, 0))
        self.fake.load_class = lambda _, path: path
        self.fake.log = lambda msg: None

    def load_script(self):
        spec = importlib.util.spec_from_file_location("sports_arcade_preview_tested", SCRIPT)
        module = importlib.util.module_from_spec(spec)
        with patch.dict(sys.modules, {"unreal": self.fake}):
            spec.loader.exec_module(module)
        return module

    def test_preview_builds_regulation_pitch_goals_ball_camera_and_lighting(self):
        module = self.load_script()
        module.main()
        self.assertEqual(len(self.editor.actors), 6)
        by_name = {a.label: a for a in self.editor.actors}
        self.assertEqual(set(by_name), {
            "SA | Regulation 105x68 Pitch", "SA | West Goal", "SA | East Goal",
            "SA | Reference Football (replace mesh)", "SA | Broadcast Camera",
            "SA | Stadium Daylight Lighting Rig",
        })
        self.assertEqual(by_name["SA | West Goal"].location.x, -5250.0)
        self.assertEqual(by_name["SA | East Goal"].location.x, 5250.0)
        self.assertEqual(by_name["SA | Reference Football (replace mesh)"].location.z, 11.0)
        self.assertEqual(by_name["SA | Broadcast Camera"].location.y, -8900.0)
        self.assertIsNotNone(by_name["SA | Broadcast Camera"].rotation)
        self.assertEqual(self.level.created, ["/Game/Maps/SA_VisualReview"])
        self.assertEqual(self.level.saved, 1)

    def test_refuses_overwriting_an_existing_review_level(self):
        self.assets.exists = True
        module = self.load_script()
        with self.assertRaisesRegex(RuntimeError, "refusing to overwrite"):
            module.main()
        self.assertEqual(self.editor.actors, [])
        self.assertEqual(self.level.created, [])

    def test_level_editor_subsystem_unavailable_aborts_without_creation(self):
        module = self.load_script()
        original = self.fake.get_editor_subsystem
        self.fake.get_editor_subsystem = lambda cls: (
            None if cls is self.fake.LevelEditorSubsystem else original(cls)
        )
        with self.assertRaisesRegex(RuntimeError, "LevelEditorSubsystem not available"):
            module.main()
        self.assertEqual(self.editor.actors, [])
        self.assertEqual(self.level.created, [])

    def test_missing_unreal_cpp_class_aborts_before_modifying_level(self):
        module = self.load_script()
        self.fake.load_class = lambda _, path: None if path.endswith("SportsArcadeBall") else path
        with self.assertRaisesRegex(RuntimeError, "not available"):
            module.main()
        self.assertEqual(self.editor.actors, [])
        self.assertEqual(self.level.created, [])


if __name__ == "__main__":
    unittest.main()
