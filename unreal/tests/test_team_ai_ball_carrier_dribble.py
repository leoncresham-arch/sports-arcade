"""Source contracts for early AI controlled dribbling; not native engine tests."""
from pathlib import Path
import unittest
SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp"

class BallCarrierDribbleTests(unittest.TestCase):
    def test_challenged_carrier_has_teammate_outlet(self):
        source = SOURCE.read_text(encoding="utf-8")
        block = source.split("if (bSelfControllingBall && bImmediateChallenge", 1)[1].split(
            "if (bSelfControllingBall && !bImmediateChallenge", 1)[0]
        self.assertIn("Mate->TeamIndex != Footballer->TeamIndex", block)
        self.assertIn("BestOutlet = Mate", block)
        self.assertIn("Ball->KickByPlayer(Footballer, Direction", block)
        self.assertIn("OutletScore > BestOutletScore", block)
        self.assertIn("Delta.X * AttackSign * 0.35f", block)
        self.assertIn("FMath::Square(160.f)", block)
        self.assertIn("FMath::Clamp(950.f + OutletDistanceCm * 0.22f, 1000.f, 1550.f)", block)
        self.assertIn("FVector2D::DotProduct", block)
        self.assertIn("Start + Path * Along", block)
        self.assertIn("if (bReceiverMarked) continue", block)
        self.assertIn("ActionCooldownRemaining = FMath::Max", block)

    def test_nearby_challenge_holds_controlled_touch(self):
        code = SOURCE.read_text(encoding="utf-8")
        self.assertIn("FMath::Clamp(DribbleChallengeRadiusCm, 50.f, 400.f)", code)
        header = SOURCE.parents[1] / "Public/SportsArcadeTeamAIController.h"
        self.assertIn("DribbleChallengeRadiusCm = 150.f", header.read_text(encoding="utf-8"))
        self.assertIn("bImmediateChallenge = true", code)
        self.assertIn("!bImmediateChallenge && ActionCooldownRemaining <= 0.f", code)

    def test_dribble_steers_away_from_nearest_defender(self):
        code = SOURCE.read_text(encoding="utf-8")
        part = code.split("if (bSelfControllingBall && !bImmediateChallenge && ActionCooldownRemaining <= 0.f)", 1)[1].split(
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
        self.assertIn("bSelfControllingBall && !bImmediateChallenge && ActionCooldownRemaining <= 0.f", section)
        self.assertIn("Ball->ApplyControlledTouch(Footballer", section)
        self.assertIn("Target = Ball->GetActorLocation() + FVector(AttackSign * 280.f", section)
        self.assertIn("bIsPrimaryPresser = true", section)
        self.assertIn("ActionCooldownRemaining = FMath::Max", section)

if __name__ == "__main__":
    unittest.main()
