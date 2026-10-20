"""Reference cases for nonauthoritative penalty ball-flight detection.

This Python model checks intended geometry but does not execute Unreal physics.
"""
from pathlib import Path
import unittest

SOURCE = (Path(__file__).resolve().parents[1] /
          "Source/SportsArcade/Private/SportsArcadeGameMode.cpp").read_text()


def observe_segment(start, end, direction=1, half_length=5250, half_width=3400,
                    goal_half_width=366, bar_height=244, radius=11):
    plane = direction * (half_length + radius)
    delta = [b - a for a, b in zip(start, end)]
    if abs(delta[0]) > 1e-6:
        t = (plane - start[0]) / delta[0]
        if 0 <= t <= 1 and delta[0] * direction > 0:
            y = start[1] + delta[1] * t
            z = start[2] + delta[2] * t
            edge = half_width + radius
            side_first = abs(start[1]) > edge
            if abs(delta[1]) > 1e-6:
                side_t = ((edge if delta[1] > 0 else -edge) - start[1]) / delta[1]
                side_first |= 0 <= side_t < t
            return int(not side_first and abs(y) + radius < goal_half_width
                       and radius <= z and z + radius < bar_height)
    if abs(end[1]) > half_width + radius:
        return 0
    return None


class PenaltyFlightObservationTests(unittest.TestCase):
    def test_geometry_cases(self):
        self.assertEqual(observe_segment((5100, 0, 20), (5300, 0, 20)), 1)
        self.assertEqual(observe_segment((5100, 400, 20), (5300, 400, 20)), 0)
        self.assertEqual(observe_segment((5100, 0, 260), (5300, 0, 260)), 0)
        self.assertEqual(observe_segment((4000, 3000, 20), (5500, 3800, 20)), 0)
        self.assertEqual(observe_segment((-5100, 0, 20), (-5300, 0, 20), -1), 1)

    def test_settled_ball_timeout_is_suggestion_only(self):
        self.assertIn("PhysicalPenaltyObservationSeconds >= PhysicalPenaltyObservationTimeoutSeconds", SOURCE)
        self.assertIn("Current.X * GoalDirection < -(PitchHalfLengthCm + MatchBall->BallRadiusCm)", SOURCE)
        self.assertIn("MatchBall->GetVelocity().SizeSquared() < FMath::Square(60.f)", SOURCE)
        self.assertIn("FMath::IsFinite(DeltaSeconds) && DeltaSeconds > 0.f", SOURCE)
        self.assertEqual(SOURCE.count("PhysicalPenaltyObservationSeconds = 0.f;"), 5)

    def test_source_checks_crossing_order_and_never_auto_adjudicates(self):
        method = SOURCE.split("void ASportsArcadeGameMode::ObservePhysicalCupPenalty()", 1)[1].split("\n}", 1)[0]
        self.assertIn("TouchlineFraction >= 0.f && TouchlineFraction < CrossingFraction", method)
        self.assertIn("SuggestedCupPenaltyOutcome = !bTouchlineFirst && bInsidePosts && bUnderBar", method)
        self.assertIn("OnCupPenaltyOutcomeSuggested.Broadcast(", method)
        self.assertNotIn("RecordCupPenaltyAttempt(", method)


if __name__ == "__main__":
    unittest.main()
