"""Source checks for teammate-controlled ball pressing gate."""
from pathlib import Path
import unittest

SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp"

class PossessionPressureTests(unittest.TestCase):
    def test_defensive_marking_does_not_override_attacking_support(self):
        code = SOURCE.read_text(encoding="utf-8")
        section = code.split("// A conservative defensive shadow", 1)[1].split(
            "ASportsArcadeCharacter* Opponent = nullptr", 1
        )[0]
        self.assertIn("Ball->GetLastTouchTeamIndex() != Footballer->TeamIndex", section)

    def test_controlled_carrier_does_not_self_press(self):
        cpp = SOURCE.read_text(encoding="utf-8")
        segment = cpp.split("const bool bSelfControllingBall", 1)[1].split(
            "if (!bTeammateNearbyBall && !bSelfControllingBall", 1)[1]
        self.assertIn("Ball->GetLastTouchPlayer() == Footballer", segment)
        self.assertIn("FMath::Square(220.f)", segment)
        self.assertIn("Nearest == Footballer", segment)

    def test_primary_ai_does_not_press_nearby_recent_teammate_ball(self):
        code = SOURCE.read_text(encoding="utf-8")
        section = code.split("const bool bTeammateNearbyBall", 1)[1].split(
            "bIsPrimaryPresser = true", 1)[0]
        self.assertIn("RecentTouchTeam == Footballer->TeamIndex", section)
        self.assertIn("IsValid(Ball->GetLastTouchPlayer())", section)
        self.assertIn("Ball->GetLastTouchPlayer() != Footballer", section)
        self.assertIn("!Ball->GetLastTouchPlayer()->GetActorLocation().ContainsNaN()", section)
        self.assertIn("FMath::Square(220.f)", section)
        self.assertIn("if (!bTeammateNearbyBall && !bSelfControllingBall", code)

if __name__ == "__main__":
    unittest.main()
