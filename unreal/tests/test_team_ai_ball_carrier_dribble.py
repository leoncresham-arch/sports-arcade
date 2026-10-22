"""Source contracts for early AI controlled dribbling; not native engine tests."""
from pathlib import Path
import unittest
SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp"

class BallCarrierDribbleTests(unittest.TestCase):
    def test_defender_distance_selection_has_deadband(self):
        source = SOURCE.read_text(encoding="utf-8")
        self.assertIn("if (DistanceSquared + 1.f < ThreatDistanceSquared)", source)

    def test_near_central_defender_favors_infield_escape(self):
        source = SOURCE.read_text(encoding="utf-8")
        self.assertIn("FMath::Abs(ThreatOffset.Y) <= 60.f", source)
        self.assertIn("Ball->GetActorLocation().Y >= 0.f ? -1.f : 1.f", source)

    def test_escape_displacement_requires_finite_coordinates(self):
        source = SOURCE.read_text(encoding="utf-8")
        self.assertIn("FMath::IsFinite(BoundedLaneY) && FMath::IsFinite(CurrentLaneY)", source)

    def test_pressure_escape_clamps_actual_lateral_displacement(self):
        source = SOURCE.read_text(encoding="utf-8")
        self.assertIn("const float BoundedLaneY = FMath::Clamp(DesiredLaneY,", source)
        self.assertIn("-CarrierSidelineLimit, CarrierSidelineLimit", source)
        self.assertIn("Target.Y += BoundedLaneY - CurrentLaneY;", source)

    def test_scaled_escape_rechecks_custom_touchline(self):
        source = SOURCE.read_text(encoding="utf-8")
        self.assertIn("const float CarrierSidelineLimit = FMath::Max(0.f, HalfWidth - 250.f)", source)
        self.assertIn("const float ProposedEscapeY =", source)
        self.assertIn("FMath::Abs(ProposedEscapeY) > CarrierSidelineLimit", source)

    def test_elevated_defender_does_not_distort_ground_escape(self):
        source = SOURCE.read_text(encoding="utf-8")
        self.assertIn("if (FMath::Abs(ThreatOffset.Z) > 150.f) continue;", source)
        self.assertLess(source.index("FMath::Abs(ThreatOffset.Z) > 150.f"),
                        source.index("const float EscapeLaneOffsetCm ="))

    def test_carrier_escape_scales_with_frontal_pressure(self):
        source = SOURCE.read_text(encoding="utf-8")
        self.assertIn("const float EscapeLaneOffsetCm =", source)
        self.assertIn("FMath::Sqrt(ThreatDistanceSquared)", source)
        self.assertIn("180.f, 320.f", source)
        self.assertIn("Target.Y += BoundedLaneY - CurrentLaneY;", source)

    def test_challenged_carrier_has_teammate_outlet(self):
        source = SOURCE.read_text(encoding="utf-8")
        block = source.split("if (bSelfControllingBall && bImmediateChallenge", 1)[1].split(
            "if (bSelfControllingBall && !bImmediateChallenge", 1)[0]
        self.assertIn("Mate->TeamIndex != Footballer->TeamIndex", block)
        self.assertIn("const FVector PredictedOutlet = Ball->GetActorLocation() + Delta", block)
        self.assertIn("PredictedOutlet.ContainsNaN()", block)
        self.assertIn("Delta.ContainsNaN() || DistanceSq <= KINDA_SMALL_NUMBER", block)
        self.assertIn("FMath::Abs(PredictedOutlet.X) > HalfLength - 100.f", block)
        self.assertIn("FMath::Abs(PredictedOutlet.Y) > HalfWidth - 100.f", block)
        self.assertIn("BestOutlet = Mate", block)
        self.assertIn("Ball->KickByPlayer(Footballer, Direction", block)
        self.assertIn("OutletScore > BestOutletScore", block)
        self.assertIn("SidelinePenalty", block)
        self.assertIn("HalfWidth - 450.f", block)
        self.assertIn("Delta.X * AttackSign * 0.35f", block)
        self.assertIn("FMath::Square(160.f)", block)
        self.assertIn("FMath::Clamp(950.f + OutletDistanceCm * 0.22f, 1000.f, 1550.f)", block)
        self.assertIn("FVector2D::DotProduct", block)
        self.assertIn("Start + Path * Along", block)
        self.assertIn("MateVelocity.ContainsNaN()", block)
        self.assertIn("MateVelocity.GetClampedToMaxSize2D(650.f) * 0.20f", block)
        self.assertIn("const FVector2D End(Ball->GetActorLocation() + Delta)", block)
        self.assertIn("BestOutletTarget = Ball->GetActorLocation() + Delta", block)
        self.assertIn("const FVector ReleaseDelta = BestOutletTarget - Ball->GetActorLocation()", block)
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
        part = code.split("if (bSelfControllingBall && !bImmediateChallenge && ActionCooldownRemaining <= 0.f &&", 1)[1].split(
            "if (!bTeammateNearbyBall", 1)[0]
        self.assertIn("const float SafeLateralSign = CarrierEscapeSide", part)
        self.assertIn("ThreatOffset.X * AttackSign < -100.f", code)
        self.assertIn("(ThreatOffset.Y >= 0.f ? -1.f : 1.f)", code)
        self.assertIn("const FVector DribbleDirection(AttackSign, SafeLateralSign * 0.6f, 0.f)", part)
        self.assertIn("Target.Y += BoundedLaneY - CurrentLaneY;", code)
        self.assertIn("HalfWidth - 250.f", code)
        self.assertIn("CarrierEscapeSide = Ball->GetActorLocation().Y >= 0.f ? -1.f : 1.f", code)
        self.assertIn("bDribbleLaneCrowded ? 280.f : 380.f", part)
        self.assertIn("DribbleDirection * DribbleSpeed", part)
        self.assertIn("<= 115.f", code)
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
