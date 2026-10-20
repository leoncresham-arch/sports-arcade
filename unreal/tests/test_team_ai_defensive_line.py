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


if __name__ == "__main__":
    unittest.main()
