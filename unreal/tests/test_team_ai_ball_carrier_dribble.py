"""Source contracts for early AI controlled dribbling; not native engine tests."""
from pathlib import Path
import unittest
SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp"

class BallCarrierDribbleTests(unittest.TestCase):
    def test_dribble_steers_away_from_nearest_defender(self):
        code = SOURCE.read_text(encoding="utf-8")
        part = code.split("if (bSelfControllingBall && ActionCooldownRemaining <= 0.f)", 1)[1].split(
            "if (!bTeammateNearbyBall", 1)[0]
        self.assertIn("const float SafeLateralSign = CarrierEscapeSide", part)
        self.assertIn("ThreatOffset.X * AttackSign < -100.f", code)
        self.assertIn("CarrierEscapeSide = ThreatOffset.Y >= 0.f ? -1.f : 1.f", code)
        self.assertIn("const FVector DribbleDirection(AttackSign, SafeLateralSign * 0.6f, 0.f)", part)
        self.assertIn("Target.Y += CarrierEscapeSide * 180.f", code)
        self.assertIn("HalfWidth - 250.f", code)
        self.assertIn("CarrierEscapeSide = Ball->GetActorLocation().Y >= 0.f ? -1.f : 1.f", code)
        self.assertIn("bDribbleLaneCrowded ? 280.f : 380.f", part)
        self.assertIn("DribbleDirection * DribbleSpeed", part)
        self.assertIn("CarrierEscapeSide != 0.f", code)

    def test_dribble_requires_attributed_nearby_carrier_and_cooldown(self):
        code = SOURCE.read_text(encoding="utf-8")
        section = code.split("const bool bSelfControllingBall",1)[1].split(
            "if (!bTeammateNearbyBall",1)[0]
        self.assertIn("Ball->GetLastTouchPlayer() == Footballer", section)
        self.assertIn("FMath::Square(220.f)", section)
        self.assertIn("bSelfControllingBall && ActionCooldownRemaining <= 0.f", section)
        self.assertIn("Ball->ApplyControlledTouch(Footballer", section)
        self.assertIn("Target = Ball->GetActorLocation() + FVector(AttackSign * 280.f", section)
        self.assertIn("bIsPrimaryPresser = true", section)
        self.assertIn("ActionCooldownRemaining = FMath::Max", section)

if __name__ == "__main__":
    unittest.main()
