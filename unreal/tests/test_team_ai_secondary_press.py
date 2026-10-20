"""Source-level checks for second-defender support during attributed opposition possession."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1] / "Source/SportsArcade"
HEADER = ROOT / "Public/SportsArcadeTeamAIController.h"
SOURCE = ROOT / "Private/SportsArcadeTeamAIController.cpp"

class SecondaryPressTests(unittest.TestCase):
    def test_tunable_shadowing_limits(self):
        header = HEADER.read_text(encoding="utf-8")
        self.assertIn("SecondaryPressureRadiusCm = 1700.f", header)
        self.assertIn("SecondaryPressureStandOffCm = 450.f", header)

    def test_second_defender_is_distinct_and_excludes_keeper(self):
        code = SOURCE.read_text(encoding="utf-8")
        part = code.split("// The second-closest autonomous outfielder", 1)[1].split(
            "if (Nearest == Footballer", 1
        )[0]
        self.assertIn("Nearest != Footballer", part)
        self.assertIn("Second == Footballer", part)
        self.assertIn("Slot > 4 || bDefensiveEmergency", part)
        self.assertIn("Candidate == Nearest", part)
        self.assertIn("Candidate->LineupSlot == 0", part)
        self.assertIn("Roster->Players.IndexOfByKey(Candidate) == 0", part)
        self.assertIn("RecentTouchTeam == 1 - Footballer->TeamIndex", part)
        self.assertIn("FMath::Clamp(SecondaryPressureRadiusCm, 0.f, 3000.f)", part)
        self.assertIn("FMath::Clamp(SecondaryPressureStandOffCm, 0.f, 1000.f)", part)
        self.assertIn("Candidate->GetActorLocation().ContainsNaN()", part)
        self.assertIn("Target.Y +=", part)

if __name__ == "__main__":
    unittest.main()
