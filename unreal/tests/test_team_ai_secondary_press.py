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

    def test_equal_distance_pressers_use_lineup_slot_tie_break(self):
        AI_CPP = SOURCE.read_text(encoding="utf-8")
        self.assertIn("Distance < BestDistance - 1.f", AI_CPP)
        self.assertIn("Distance < SecondDistance - 1.f", AI_CPP)
        self.assertIn("FMath::Abs(Distance - BestDistance) <= 1.f", AI_CPP)
        self.assertIn("FMath::Abs(Distance - SecondDistance) <= 1.f", AI_CPP)

    def test_second_defender_is_distinct_and_excludes_keeper(self):
        code = SOURCE.read_text(encoding="utf-8")
        part = code.split("// Rank the two nearest AI outfielders together", 1)[1].split(
            "if (Second == Footballer &&", 1
        )[0]
        self.assertIn("Nearest != Footballer", part)
        self.assertIn("Second = Nearest;", part)
        self.assertIn("Second = Candidate;", part)
        self.assertIn("Slot > 4 || bDefensiveEmergency", part)
        self.assertIn("SecondDistance = BestDistance;", part)
        self.assertIn("Candidate->LineupSlot == 0", part)
        self.assertIn("Roster->Players.IndexOfByKey(Candidate) == 0", part)
        self.assertIn("RecentTouchTeam == 1 - Footballer->TeamIndex", part)
        self.assertIn("FMath::Clamp(SecondaryPressureRadiusCm, 0.f, 3000.f)", code)
        self.assertIn("FMath::Clamp(SecondaryPressureStandOffCm, 0.f, 1000.f)", code)
        self.assertIn("Candidate->GetActorLocation().ContainsNaN()", part)
        self.assertIn("Target.Y +=", code)
        self.assertEqual(part.count("for (ASportsArcadeCharacter* Candidate : Roster->Players)"), 1)

if __name__ == "__main__":
    unittest.main()
