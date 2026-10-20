"""Source checks for teammate-controlled ball pressing gate."""
from pathlib import Path
import unittest

SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp"

class PossessionPressureTests(unittest.TestCase):
    def test_primary_ai_does_not_press_nearby_recent_teammate_ball(self):
        code = SOURCE.read_text(encoding="utf-8")
        section = code.split("const bool bTeammateNearbyBall", 1)[1].split(
            "bIsPrimaryPresser = true", 1)[0]
        self.assertIn("RecentTouchTeam == Footballer->TeamIndex", section)
        self.assertIn("IsValid(Ball->GetLastTouchPlayer())", section)
        self.assertIn("FMath::Square(220.f)", section)
        self.assertIn("if (!bTeammateNearbyBall && Nearest == Footballer", section)

if __name__ == "__main__":
    unittest.main()
