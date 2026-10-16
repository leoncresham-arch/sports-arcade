"""Mock-only Unreal Editor integration tests; never represent real UE execution."""
import importlib.util
import sys
import types
import unittest
from pathlib import Path
from unittest.mock import patch

SCRIPT = Path(__file__).resolve().parents[1] / "Tools/Editor/setup_offline_match.py"

class Actor:
    def __init__(self, name, where):
        self.name, self.location, self.props = name, where, {}
    def set_actor_label(self, label):
        self.label = label
    def set_editor_property(self, key, value):
        self.props[key] = value
    def get_formation_position(self, slot):
        direction = 1 if self.props.get("team_index") == 0 else -1
        return types.SimpleNamespace(x=direction * (-4750 + slot * 500), y=slot * 30)

class Editor:
    def __init__(self):
        self.actors = []
    def spawn_actor_from_class(self, cls, pos, rot):
        actor = Actor(cls, pos)
        self.actors.append(actor)
        return actor

class MatchBootstrapTests(unittest.TestCase):
    def setUp(self):
        self.editor = Editor()
        self.level = types.SimpleNamespace(
            new_level=lambda path: True,
            save_current_level=lambda: True)
        self.assets = types.SimpleNamespace(
            does_asset_exist=lambda path: False,
            make_directory=lambda path: True)
        self.fake = types.ModuleType("unreal")
        self.fake.EditorActorSubsystem = type("EditorActorSubsystem", (), {})
        self.fake.LevelEditorSubsystem = type("LevelEditorSubsystem", (), {})
        self.fake.get_editor_subsystem = lambda cls: (
            self.editor if cls is self.fake.EditorActorSubsystem else self.level)
        self.fake.load_class = lambda _, path: path
        self.fake.EditorAssetLibrary = self.assets
        self.fake.Vector = lambda x,y,z: types.SimpleNamespace(x=x,y=y,z=z)
        self.fake.Rotator = lambda *args: args
    def load(self):
        spec = importlib.util.spec_from_file_location("match_bootstrap", SCRIPT)
        mod = importlib.util.module_from_spec(spec)
        with patch.dict(sys.modules, {"unreal": self.fake}):
            spec.loader.exec_module(mod)
        return mod
    def test_creates_exactly_22_players_and_two_rosters(self):
        self.load().main()
        self.assertEqual(len(self.editor.actors), 28)
        rosters = [a for a in self.editor.actors if "TeamRoster" in a.name]
        self.assertEqual(len(rosters), 2)
        self.assertEqual([len(a.props["players"]) for a in rosters], [11,11])
    def test_existing_level_not_overwritten(self):
        self.assets.does_asset_exist = lambda path: True
        with self.assertRaisesRegex(RuntimeError, "Refusing to overwrite"):
            self.load().main()
        self.assertEqual(self.editor.actors, [])

if __name__ == "__main__":
    unittest.main()
