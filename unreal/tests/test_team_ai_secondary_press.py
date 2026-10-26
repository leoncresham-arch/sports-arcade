"""Source-level checks for second-defender support during attributed opposition possession."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1] / "Source/SportsArcade"
HEADER = ROOT / "Public/SportsArcadeTeamAIController.h"
SOURCE = ROOT / "Private/SportsArcadeTeamAIController.cpp"

class SecondaryPressTests(unittest.TestCase):
    def test_secondary_pressure_target_survives_marking(self):
        code = SOURCE.read_text(encoding="utf-8")
        self.assertIn("bool bIsSecondaryPresser = false;", code)
        self.assertIn("bIsSecondaryPresser = true;", code)
        self.assertIn("!bIsPrimaryPresser && !bIsSecondaryPresser", code)

    def test_tunable_shadowing_limits(self):
        header = HEADER.read_text(encoding="utf-8")
        self.assertIn("SecondaryPressureRadiusCm = 1700.f", header)
        self.assertIn("SecondaryPressureStandOffCm = 450.f", header)

    def test_selection_uses_reusable_deterministic_algorithm(self):
        code = SOURCE.read_text(encoding="utf-8")
        algorithm = (ROOT / "Private/SportsArcadeTacticalSelection.cpp").read_text()
        self.assertIn("FSportsArcadeTacticalSelection::SelectPressPair(PressCandidates)", code)
        self.assertIn("Candidate.DistanceSquared < Current.DistanceSquared - 1.f", algorithm)
        self.assertIn("Candidate.Slot < Current.Slot", algorithm)

    def test_second_defender_is_distinct_and_excludes_keeper(self):
        code = SOURCE.read_text(encoding="utf-8")
        self.assertIn("Candidate->LineupSlot == 0", code)
        self.assertIn("Roster->Players.IndexOfByKey(Candidate) == 0", code)
        self.assertIn("Cast<ASportsArcadeTeamAIController>(Candidate->GetController())", code)
        self.assertIn("PressPair.CoverSlot", code)
        self.assertIn("Nearest != Footballer", code)
        self.assertIn("Second == Footballer", code)
        self.assertIn("RecentTouchTeam == 1 - Footballer->TeamIndex", code)
        self.assertIn("FMath::Clamp(SecondaryPressureRadiusCm, 0.f, 3000.f)", code)
        self.assertIn("FMath::Clamp(SecondaryPressureStandOffCm, 0.f, 1000.f)", code)
        self.assertIn("Target.Y +=", code)

if __name__ == "__main__":
    unittest.main()
