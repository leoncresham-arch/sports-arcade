"""Source contract for cautious, proximity-validated possession attribution."""
from pathlib import Path
import unittest

SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp"

class PossessionProximityTests(unittest.TestCase):
    def test_loose_ball_does_not_trigger_team_possession_shift(self):
        cpp = SOURCE.read_text(encoding="utf-8")
        section = cpp.split("// A last touch without nearby control", 1)[1].split(
            "const float AttackSign", 1)[0]
        self.assertIn("GetLastTouchPlayer()", section)
        self.assertIn("LastToucher->GetWorld() == GetWorld()", section)
        self.assertIn("FMath::Square(300.f)", section)
        self.assertIn("bAttributedControl", section)
        self.assertIn("Ball->GetLastTouchTeamIndex() : INDEX_NONE", section)

if __name__ == "__main__":
    unittest.main()
