"""Structural checks for read-only Unreal Editor validation helper."""
from pathlib import Path
import unittest

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

if __name__ == "__main__":
    unittest.main()
