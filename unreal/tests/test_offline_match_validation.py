"""Structural checks for read-only Unreal Editor validation helper."""
from pathlib import Path
import unittest
import importlib.util
import types
import sys
from unittest.mock import patch

SCRIPT = Path(__file__).resolve().parents[1] / "Tools/Editor/validate_offline_match.py"

class OfflineValidationContracts(unittest.TestCase):
    def test_requirements_and_no_mutation(self):
        code = SCRIPT.read_text()
        self.assertIn("get_all_level_actors()", code)
        self.assertIn("Expected one match ball", code)
        self.assertIn("Expected two team rosters", code)
        self.assertIn("Expected 22 footballers", code)
        self.assertIn("sorted(slots) != list(range(11))", code)
        self.assertNotIn("spawn_actor_from_class(", code)
        self.assertNotIn("save_current_level(", code)
        self.assertNotIn("destroy_actor(", code)

    def test_valid_and_missing_roster_scenarios(self):
        class Actor:
            def __init__(self, name, **props):
                self.kind = name
                self.props = props
            def get_class(self):
                return types.SimpleNamespace(get_name=lambda: self.kind)
            def get_editor_property(self, key):
                return self.props[key]

        players = [Actor("SportsArcadeCharacter", team_index=team,
                         lineup_slot=slot) for team in (0, 1) for slot in range(11)]
        rosters = [Actor("SportsArcadeTeamRoster", team_index=team,
                         players=players[team * 11:(team + 1) * 11]) for team in (0, 1)]
        actors = players + rosters + [Actor("SportsArcadeBall")]
        fake = types.ModuleType("unreal")
        fake.EditorActorSubsystem = object()
        fake.get_editor_subsystem = lambda cls: types.SimpleNamespace(get_all_level_actors=lambda: actors)
        fake.log = lambda message: None
        fake.log_error = lambda message: None
        spec = importlib.util.spec_from_file_location("offline_match_validate_test", SCRIPT)
        module = importlib.util.module_from_spec(spec)
        with patch.dict(sys.modules, {"unreal": fake}):
            spec.loader.exec_module(module)
        self.assertEqual(module.validate(), [])
        actors.pop()
        self.assertTrue(any("match ball" in message for message in module.validate()))

if __name__ == "__main__":
    unittest.main()
