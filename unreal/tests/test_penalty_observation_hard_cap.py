"""Source-contract regression for prolonged moving shootout attempts.

These tests do not compile or execute Unreal Engine.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
HEADER = ROOT / "Source/SportsArcade/Public/SportsArcadeGameMode.h"
SOURCE = ROOT / "Source/SportsArcade/Private/SportsArcadeGameMode.cpp"


class PenaltyObservationHardCapTests(unittest.TestCase):
    def test_editor_tunable_hard_cap_is_separate_from_settled_ball_timeout(self):
        header = HEADER.read_text(encoding="utf-8")
        self.assertIn("PhysicalPenaltyObservationTimeoutSeconds = 8.f", header)
        self.assertIn("PhysicalPenaltyMaximumObservationSeconds = 20.f", header)
        self.assertIn("meta=(ClampMin=\"1.0\")", header)

    def test_moving_shot_eventually_requires_referee_review(self):
        source = SOURCE.read_text(encoding="utf-8")
        observer = source.split("void ASportsArcadeGameMode::ObservePhysicalCupPenalty()", 1)[1].split(
            "bool ASportsArcadeGameMode::WasCupPenaltyTouchedByKeeper()", 1
        )[0]
        self.assertIn(
            "PhysicalPenaltyObservationSeconds >= FMath::Max(1.f, PhysicalPenaltyMaximumObservationSeconds)",
            observer,
        )
        self.assertIn(
            "PhysicalPenaltyObservationSeconds >= PhysicalPenaltyObservationTimeoutSeconds",
            observer,
        )
        self.assertIn("MatchBall->GetVelocity().SizeSquared() < FMath::Square(60.f)", observer)
        self.assertRegex(observer, r"SuggestedCupPenaltyOutcome = 0;")
        self.assertIn("OnCupPenaltyOutcomeSuggested.Broadcast", observer)
        self.assertNotIn("RecordCupPenaltyAttempt(", observer)
        self.assertNotIn("ResolvePhysicalCupPenalty(", observer)

    def test_goal_crossing_is_evaluated_before_timeouts(self):
        source = SOURCE.read_text(encoding="utf-8")
        observer = source.split("void ASportsArcadeGameMode::ObservePhysicalCupPenalty()", 1)[1].split(
            "bool ASportsArcadeGameMode::WasCupPenaltyTouchedByKeeper()", 1
        )[0]
        self.assertLess(observer.index("const float CrossingFraction"), observer.index(
            "PhysicalPenaltyMaximumObservationSeconds"
        ))


if __name__ == "__main__":
    unittest.main()
