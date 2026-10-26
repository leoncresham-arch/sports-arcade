"""Source checks for teammate-controlled ball pressing gate."""
from pathlib import Path
import unittest

SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp"

class PossessionPressureTests(unittest.TestCase):
    def test_midfield_uses_single_opponent_snapshot_for_pressure_and_outlets(self):
        code = SOURCE.read_text(encoding="utf-8")
        self.assertIn("TArray<ASportsArcadeCharacter*> TacticalOpponents;", code)
        self.assertIn("TacticalOpponents.Add(Candidate);", code)
        self.assertIn("for (ASportsArcadeCharacter* Threat : TacticalOpponents)", code)
        self.assertIn("for (ASportsArcadeCharacter* Blocker : TacticalOpponents)", code)
        self.assertIn("for (ASportsArcadeCharacter* LaneIt : TacticalOpponents)", code)

    def test_midfield_support_runs_remain_bounded_and_possession_based(self):
        code = SOURCE.read_text(encoding="utf-8")
        part = code.split("// Central midfielders supply a short forward", 1)[1].split(
            "// Hold the authored back line", 1
        )[0]
        self.assertIn("RecentTouchTeam == Footballer->TeamIndex", part)
        self.assertIn("Slot >= 5 && Slot <= 8", part)
        self.assertIn("FMath::Square(2400.f)", part)
        self.assertIn("FMath::Clamp(SupportGap * 0.25f, 0.f, 450.f)", part)
        self.assertIn("ForwardSupportScale", part)
        self.assertIn("SupportAdvance * ForwardSupportScale", part)
        self.assertIn("AttackSign * 175.f", part)
        self.assertIn("FMath::Min(Target.X, MaximumSupportX)", part)
        self.assertIn("FMath::Max(Target.X, MaximumSupportX)", part)
        self.assertIn("FMath::Abs(LateralSeparation) < 250.f", part)
        self.assertIn("Slot % 2 == 0 ? 1.f : -1.f", part)
        self.assertIn("FMath::Sign(LateralSeparation)", part)
        self.assertIn("Target.Y = Ball->GetActorLocation().Y + SideSign * 250.f", part)
        self.assertIn("FMath::Abs(Ball->GetActorLocation().Y) > HalfWidth - 600.f", part)
        self.assertIn("FMath::Sign(Ball->GetActorLocation().Y) * 350.f", part)
        self.assertIn("const FVector MidfieldBase = Roster->GetFormationPosition(Slot)", part)
        self.assertIn("MidfieldBaseY - 1100.f, MidfieldBaseY + 1100.f", part)
        self.assertIn("const FVector OutletDelta", part)
        self.assertIn("BlockerVelocity.SizeSquared2D() > FMath::Square(380.f)", part)
        self.assertIn("OutletDelta.SizeSquared2D() >= FMath::Square(190.f)", part)
        self.assertIn("Blocker->GetActorLocation().Y + ClearSide * 220.f", part)
        self.assertIn("FMath::Abs(PlayableClearY - Blocker->GetActorLocation().Y) >= 175.f", part)
        self.assertIn("const float PlayableClearY = FMath::Clamp(ClearY,", part)
        self.assertIn("-HalfWidth + 180.f, HalfWidth - 180.f", part)
        self.assertIn("bRetreatingPass", part)
        self.assertIn("bFastLateralSwitch", part)
        self.assertIn("bCarrierUnderPressure", part)
        self.assertIn("bThreatReceding", part)
        self.assertIn("bThreatClosing", part)
        self.assertIn("!RelativeThreatVelocity.ContainsNaN()", part)
        self.assertIn("ThreatVelocity - SupportBallVelocity", part)
        self.assertIn("ThreatVelocity.SizeSquared2D() > FMath::Square(350.f)", part)
        self.assertIn("FMath::Abs(ThreatDelta.Z) <= 135.f", part)
        self.assertIn("FMath::Square(220.f)", part)
        self.assertIn("FMath::Abs(SupportBallVelocity.Y) > 1100.f", part)
        self.assertIn("SupportBallVelocity.X * AttackSign < -450.f", part)
        self.assertIn("FMath::ClosestPointOnSegment(", part)
        self.assertIn("FMath::Square(145.f)", part)
        self.assertIn("const float CandidateY", part)
        self.assertIn("const float SupportDepth =", part)
        self.assertIn("if (SupportDepth < 175.f)", part)
        self.assertLess(part.index("if (FMath::Abs(Ball->GetActorLocation().Y) > HalfWidth - 600.f)"), len(part))
        self.assertIn("}\n            // A wide ball carrier always needs an infield outlet", part)

    def test_off_ball_marking_protects_central_lane(self):
        code = SOURCE.read_text(encoding="utf-8")
        section = code.split("if (IsValid(Opponent))", 1)[1]
        self.assertIn("InfieldMarkingShift", section)
        self.assertIn("FMath::Abs(MarkingPoint.Y) - 1100.f", section)
        self.assertIn("CentralCoverY", section)
        self.assertIn("GoalDistance / 1200.f, 0.35f, 1.f", section)
        self.assertIn("bRunnerAttackingGoal", section)
        self.assertIn("InfieldMarkingShift * UrgentGoalCover", section)
        self.assertIn("MarkingPoint.Y - CentralCoverY", section)
        self.assertIn("FMath::Clamp(MarkingInfluence, 0.f, 0.5f)", section)

    def test_team_support_does_not_claim_airborne_or_separated_ball(self):
        code = SOURCE.read_text(encoding="utf-8")
        ball = (SOURCE.parent / "SportsArcadeBall.cpp").read_text()
        self.assertIn("Ball->GetControlledPlayer()", code)
        self.assertIn("MaximumHeightDifferenceCm", ball)
        self.assertIn("FVector::DistSquared2D(Player->GetActorLocation()", ball)

    def test_defensive_marking_does_not_override_attacking_support(self):
        code = SOURCE.read_text(encoding="utf-8")
        section = code.split("// A conservative defensive shadow", 1)[1].split(
            "ASportsArcadeCharacter* Opponent = nullptr", 1
        )[0]
        self.assertIn("!bTeammateControlsForMarking", section)
        self.assertIn("MarkingCarrier->TeamIndex == Footballer->TeamIndex", code)

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
        self.assertIn("Ball->GetControlledPlayer(220.f, 115.f)", cpp)
        self.assertIn("const bool bSelfControllingBall = CloseCarrier == Footballer;", cpp)
        self.assertIn("!bSelfControllingBall", cpp)

    def test_primary_ai_does_not_press_nearby_recent_teammate_ball(self):
        cpp = SOURCE.read_text(encoding="utf-8")
        self.assertIn("CloseCarrier != Footballer", cpp)
        self.assertIn("CloseCarrier->TeamIndex == Footballer->TeamIndex", cpp)
        self.assertIn("Ball->GetControlledPlayer(220.f, 115.f)", cpp)
        self.assertIn("if (!bTeammateNearbyBall && !bSelfControllingBall", cpp)

if __name__ == "__main__":
    unittest.main()
