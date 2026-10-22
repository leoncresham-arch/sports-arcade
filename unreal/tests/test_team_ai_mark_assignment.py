"""Static contracts for coordinated autonomous defensive marking (not UE runtime tests)."""
from pathlib import Path
import unittest

SOURCE = (Path(__file__).resolve().parents[1] / "Source" / "SportsArcade" /
          "Private" / "SportsArcadeTeamAIController.cpp").read_text(encoding="utf-8")


class TeamMarkAssignmentTests(unittest.TestCase):
    def test_assignment_only_in_defensive_marking_block(self):
        marker = SOURCE.index("// A conservative defensive shadow")
        ownership = SOURCE.index("bAnotherTeammateOwnsMark")
        fallback = SOURCE.index("if (IsValid(Opponent))", marker)
        self.assertLess(marker, ownership)
        self.assertLess(ownership, fallback)

    def test_closest_eligible_ai_teammate_owns_opponent(self):
        self.assertIn("Teammate == Footballer", SOURCE)
        self.assertIn("Teammate->GetWorld() != GetWorld()", SOURCE)
        self.assertIn("Teammate->TeamIndex != Footballer->TeamIndex", SOURCE)
        self.assertIn("Cast<ASportsArcadeTeamAIController>(Teammate->GetController())", SOURCE)
        self.assertIn("if (TeammateSlot == 0 || TeammateSlot < 0 || TeammateSlot > 10)", SOURCE)
        self.assertIn("TheirDistance + 1.f < Distance", SOURCE)
        self.assertIn("TeammateSlot < Slot", SOURCE)
        self.assertIn("if (bAnotherTeammateOwnsMark) continue;", SOURCE)

    def test_other_runner_coverage_not_suppressed_by_busy_teammate(self):
        self.assertIn("bTeammateHasCloserThreat", SOURCE)
        self.assertIn("TActorIterator<ASportsArcadeCharacter> OtherThreat(GetWorld())", SOURCE)
        self.assertIn("OtherThreat->TeamIndex == Footballer->TeamIndex", SOURCE)
        self.assertIn("AlternativeDistance + FMath::Square(75.f) < TheirDistance", SOURCE)
        self.assertIn("if (!bTeammateHasCloserThreat)", SOURCE)

    def test_off_pitch_opponents_are_not_marking_targets(self):
        self.assertIn("const FVector ThreatPosition = It->GetActorLocation();", SOURCE)
        self.assertIn("FMath::Abs(ThreatPosition.X) > HalfLength + 150.f", SOURCE)
        self.assertIn("FMath::Abs(ThreatPosition.Y) > HalfWidth + 150.f", SOURCE)

    def test_final_navigation_target_uses_player_ground_plane(self):
        self.assertIn("Target.Z = CurrentPlayerPosition.Z;", SOURCE)
        self.assertLess(SOURCE.rindex("Target.Z = CurrentPlayerPosition.Z;"),
                        SOURCE.rindex("MoveToLocation(Target, 125.f"))

    def test_reassessment_horizon_bounds_ai_navigation(self):
        self.assertIn("const float MaximumDecisionTargetDistanceCm = 1800.f", SOURCE)
        self.assertIn("PlanarTargetDelta.GetClampedToMaxSize2D(", SOURCE)
        self.assertIn("CurrentPlayerPosition.X + BoundedDelta.X", SOURCE)
        self.assertIn("CurrentPlayerPosition.Y + BoundedDelta.Y", SOURCE)
        self.assertIn("Target.X = FMath::Clamp(CurrentPlayerPosition.X", SOURCE)

    def test_small_pitch_uses_nonnegative_navigation_extents(self):
        self.assertIn("const float SafeHalfLength = FMath::Max(0.f, HalfLength - 150.f)", SOURCE)
        self.assertIn("const float SafeHalfWidth = FMath::Max(0.f, HalfWidth - 150.f)", SOURCE)
        self.assertIn("FMath::Clamp(Target.X, -SafeHalfLength, SafeHalfLength)", SOURCE)
        self.assertIn("FMath::Clamp(Target.Y, -SafeHalfWidth, SafeHalfWidth)", SOURCE)

    def test_goal_side_and_formation_cover_preserved(self):
        self.assertIn("const FVector GoalSideMarkingPoint", SOURCE)
        self.assertIn("FMath::Clamp(MarkingInfluence, 0.f, 0.5f)", SOURCE)
        self.assertIn("if (Target.ContainsNaN()", SOURCE)


if __name__ == "__main__":
    unittest.main()
