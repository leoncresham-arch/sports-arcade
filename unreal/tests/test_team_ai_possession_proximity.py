"""Source contract for cautious, proximity-validated possession attribution."""
from pathlib import Path
import unittest

SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp"

class PossessionProximityTests(unittest.TestCase):
    def test_loose_ball_does_not_trigger_team_possession_shift(self):
        cpp = SOURCE.read_text(encoding="utf-8")
        section = cpp.split("// A last touch without nearby control", 1)[1].split(
            "const float AttackSign", 1)[0]
        self.assertIn("Ball->GetControlledPlayer()", section)
        self.assertIn("? LastToucher->TeamIndex : INDEX_NONE;", section)

    def test_marking_resumes_after_last_toucher_separates(self):
        cpp = SOURCE.read_text(encoding="utf-8")
        section = cpp.split("// A conservative defensive shadow", 1)[1].split(
            "ASportsArcadeCharacter* Opponent", 1)[0]
        self.assertIn("!bTeammateControlsForMarking", section)
        self.assertIn("Ball->GetControlledPlayer() : nullptr", cpp)
        self.assertIn("MarkingCarrier->TeamIndex == Footballer->TeamIndex", cpp)

if __name__ == "__main__":
    unittest.main()
