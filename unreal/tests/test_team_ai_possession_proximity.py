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
        self.assertIn("FMath::Square(220.f)", section)
        self.assertIn("Ball->GetActorLocation().Z) <= 135.f", section)
        self.assertIn("bAttributedControl", section)
        self.assertIn("LastToucher->TeamIndex : INDEX_NONE", section)

    def test_marking_resumes_after_last_toucher_separates(self):
        cpp = SOURCE.read_text(encoding="utf-8")
        section = cpp.split("// A conservative defensive shadow", 1)[1].split(
            "ASportsArcadeCharacter* Opponent", 1)[0]
        self.assertIn("Ball->GetLastTouchTeamIndex() != Footballer->TeamIndex", section)
        self.assertIn("Ball->GetLastTouchPlayer()->GetActorLocation().ContainsNaN()", section)
        self.assertIn("FMath::Square(300.f)", section)

if __name__ == "__main__":
    unittest.main()
