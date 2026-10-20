"""Source contracts for normal-play goalkeeper prediction and safe pitch geometry.

These checks are not Unreal compilation or gameplay simulation.
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp"


class GoalkeeperMatchAnticipationTests(unittest.TestCase):
    def test_normal_play_keeper_uses_incoming_trajectory(self):
        source = SOURCE.read_text(encoding="utf-8")
        segment = source.split("if (IsValid(Ball) && Slot == 0)", 1)[1].split(
            "else if (IsValid(Ball))", 1
        )[0]
        self.assertIn("BallPosition.ContainsNaN() || RawBallVelocity.ContainsNaN()", segment)
        self.assertIn("GoalkeeperShotMinSpeedCmPerSec", segment)
        self.assertIn("(Footballer->GetActorLocation().X - BallPosition.X) / BallVelocity.X", segment)
        self.assertIn("ApproachSeconds >= 0.f && ApproachSeconds <= 1.5f", segment)
        self.assertIn("BallPosition.Y + BallVelocity.Y * ApproachSeconds", segment)
        self.assertIn("GoalHalfWidthCm - 55.f", segment)
        self.assertIn("FMath::Clamp(AnticipatedY, -LateralRange, LateralRange)", segment)
        self.assertIn("bIncomingShot", segment)
        self.assertIn("Ball->KickByPlayer(Footballer", segment)

    def test_goalkeeper_does_not_accidentally_duplicate_towards_play(self):
        source = SOURCE.read_text(encoding="utf-8")
        segment = source.split("if (IsValid(Ball) && Slot == 0)", 1)[1].split(
            "else if (IsValid(Ball))", 1
        )[0]
        self.assertEqual(segment.count("const float TowardsPlay"), 1)
        self.assertEqual(segment.count("const bool bIncomingShot"), 1)


if __name__ == "__main__":
    unittest.main()
