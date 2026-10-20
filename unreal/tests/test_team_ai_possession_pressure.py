"""Source checks for teammate-controlled ball pressing gate."""
from pathlib import Path
import unittest

SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp"

class PossessionPressureTests(unittest.TestCase):
    def test_off_ball_marking_protects_central_lane(self):
        code = SOURCE.read_text(encoding="utf-8")
        section = code.split("if (IsValid(Opponent))", 1)[1]
        self.assertIn("InfieldMarkingShift", section)
        self.assertIn("FMath::Abs(MarkingPoint.Y) - 1100.f", section)
        self.assertIn("CentralCoverY", section)
        self.assertIn("MarkingPoint.Y - CentralCoverY", section)
        self.assertIn("FMath::Clamp(MarkingInfluence, 0.f, 0.5f)", section)

    def test_defensive_marking_does_not_override_attacking_support(self):
        code = SOURCE.read_text(encoding="utf-8")
        section = code.split("// A conservative defensive shadow", 1)[1].split(
            "ASportsArcadeCharacter* Opponent = nullptr", 1
        )[0]
        self.assertIn("Ball->GetLastTouchTeamIndex() != Footballer->TeamIndex", section)

    def test_secondary_presser_shadows_within_pitch_bounds(self):
        cpp = SOURCE.read_text(encoding="utf-8")
        section = cpp.split("if (Second == Footballer &&", 1)[1].split("bIsSecondaryPresser = true", 1)[0]
        self.assertIn("SafeBallVelocity * SupportLeadSeconds", section)
        self.assertIn("bAirborneSupport", section)
        self.assertIn("SafeBallVelocity.Z > 100.f ? 0.f : 0.10f", section)
        self.assertIn("Ball->GetVelocity().GetClampedToMaxSize2D(1600.f)", section)
        self.assertIn("Target = BallLead - FVector(AttackSign * StandOff", section)
        self.assertIn("FMath::Max(StandOff * 0.5f, 120.f)", section)
        self.assertIn("const float SideSign", section)
        self.assertIn("BallLead.ContainsNaN()", section)
        self.assertIn("bBallMovingInfield = FMath::Abs(BallLead.Y) > 500.f", section)
        self.assertIn("BallLead.Y * SafeBallVelocity.Y < 0.f", section)
        self.assertIn("FMath::Abs(SafeBallVelocity.Y) > 150.f", section)
        self.assertIn("(bNearTouchline || bBallMovingInfield)", section)
        self.assertIn("bNearTouchline = FMath::Abs(BallLead.Y) > HalfWidth - 650.f", section)
        self.assertIn("BallLead.Y >= 0.f ? -1.f : 1.f", section)
        self.assertIn("Target.X = FMath::Clamp(Target.X, -HalfLength + 100.f, HalfLength - 100.f)", section)
        self.assertIn("Target.Y = FMath::Clamp(Target.Y, -HalfWidth + 100.f, HalfWidth - 100.f)", section)

    def test_controlled_carrier_does_not_self_press(self):
        cpp = SOURCE.read_text(encoding="utf-8")
        segment = cpp.split("const bool bSelfControllingBall", 1)[1].split(
            "if (!bTeammateNearbyBall && !bSelfControllingBall", 1)[0]
        self.assertIn("Ball->GetLastTouchPlayer() == Footballer", segment)
        self.assertIn("FMath::Square(220.f)", segment)
        self.assertIn("!bSelfControllingBall", cpp)

    def test_primary_ai_does_not_press_nearby_recent_teammate_ball(self):
        code = SOURCE.read_text(encoding="utf-8")
        section = code.split("const bool bTeammateNearbyBall", 1)[1].split(
            "bIsPrimaryPresser = true", 1)[0]
        self.assertIn("RecentTouchTeam == Footballer->TeamIndex", section)
        self.assertIn("IsValid(Ball->GetLastTouchPlayer())", section)
        self.assertIn("Ball->GetLastTouchPlayer() != Footballer", section)
        self.assertIn("!Ball->GetLastTouchPlayer()->GetActorLocation().ContainsNaN()", section)
        self.assertIn("FMath::Square(220.f)", section)
        self.assertIn("Ball->GetLastTouchPlayer()->GetWorld() == GetWorld()", section)
        self.assertIn("FMath::Abs(Ball->GetLastTouchPlayer()->GetActorLocation().Z -", section)
        self.assertIn("Ball->GetActorLocation().Z) <= 115.f", section)
        self.assertIn("if (!bTeammateNearbyBall && !bSelfControllingBall", code)

if __name__ == "__main__":
    unittest.main()
