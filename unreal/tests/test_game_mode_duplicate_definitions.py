"""Catch duplicate C++ GameMode implementations before attempting a native UE build."""
from collections import Counter
from pathlib import Path
import re
import unittest

CPP = (Path(__file__).resolve().parents[1] /
       "Source/SportsArcade/Private/SportsArcadeGameMode.cpp").read_text()


class NativeMethodUniqueness(unittest.TestCase):
    def test_game_mode_method_definitions_are_unique(self):
        # Restrict matching to class-qualified method declarations at line starts.
        # Do not confuse ordinary calls, comments or prototypes with definitions.
        methods = re.findall(
            r"(?m)^[^\n;{}]*\bASportsArcadeGameMode::([A-Za-z_]\w*)\s*\(",
            CPP,
        )
        self.assertIn("StageCupPenaltyKick", methods)
        self.assertIn("KickStagedCupPenalty", methods)
        duplicated = {name: count for name, count in Counter(methods).items() if count > 1}
        self.assertEqual(duplicated, {}, f"Duplicate C++ GameMode definitions: {duplicated}")


if __name__ == "__main__":
    unittest.main()
