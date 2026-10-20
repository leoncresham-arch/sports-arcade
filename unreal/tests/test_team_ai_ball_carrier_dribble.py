"""Source contracts for early AI controlled dribbling; not native engine tests."""
from pathlib import Path
import unittest
SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp"

class BallCarrierDribbleTests(unittest.TestCase):
    def test_dribble_requires_attributed_nearby_carrier_and_cooldown(self):
        code = SOURCE.read_text(encoding="utf-8")
        section = code.split("const bool bSelfControllingBall",1)[1].split(
            "if (!bTeammateNearbyBall",1)[0]
        self.assertIn("Ball->GetLastTouchPlayer() == Footballer", section)
        self.assertIn("FMath::Square(220.f)", section)
        self.assertIn("bSelfControllingBall && ActionCooldownRemaining <= 0.f", section)
        self.assertIn("Ball->ApplyControlledTouch(Footballer", section)
        self.assertIn("ActionCooldownRemaining = FMath::Max", section)

if __name__ == "__main__":
    unittest.main()
