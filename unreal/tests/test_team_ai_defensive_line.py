"""Source contracts for formation-relative defensive line discipline."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1] / "Source/SportsArcade"
HEADER = ROOT / "Public/SportsArcadeTeamAIController.h"
SOURCE = ROOT / "Private/SportsArcadeTeamAIController.cpp"


class DefensiveLineTests(unittest.TestCase):
    def test_configurable_limits(self):
        header = HEADER.read_text()
        self.assertIn("DefensiveLineMaximumAdvanceCm = 1300.f", header)
        self.assertIn("DefensiveLineRecoveryDepthCm = 650.f", header)
        self.assertIn("DefensiveLineLateralDisplacementCm = 650.f", header)

    def test_controlled_counter_has_tighter_recovery_than_loose_ball(self):
        code = SOURCE.read_text()
        self.assertIn("const bool bOpponentClearlyControllingCounter =", code)
        self.assertIn("bRapidDefensiveTransition &&", code)
        self.assertIn("TransitionToucher->TeamIndex != Footballer->TeamIndex", code)
        self.assertIn("Ball->GetActorLocation().Z) <= 135.f", code)
        self.assertIn("FMath::Square(220.f)", code)
        self.assertIn("bOpponentClearlyControllingCounter ? 1050.f :", code)
        self.assertIn("(bDefensiveHalf ? 1350.f : 1800.f)", code)

    def test_back_line_restrained_only_when_opposition_last_touched(self):
        code = SOURCE.read_text()
        section = code.split("// Hold the authored back line", 1)[1].split(
            "// Make nearby non-keeper support runners", 1)[0]
        self.assertIn("Slot >= 1 && Slot <= 4", section)
        self.assertIn("RecentTouchTeam == 1 - Footballer->TeamIndex", section)
        self.assertIn("Roster->GetFormationPosition(Slot)", section)
        self.assertIn("FMath::Clamp(RelativeX, -Retreat, Advance)", section)
        self.assertIn("DefensiveLineMaximumAdvanceCm", section)
        self.assertIn("DefensiveLineRecoveryDepthCm", section)
        self.assertIn("BasePosition.Y * HalfWidth / 3400.f", section)
        self.assertIn("FMath::Clamp(Target.Y, BaseY - LateralLimit, BaseY + LateralLimit)", section)


if __name__ == "__main__":
    unittest.main()
