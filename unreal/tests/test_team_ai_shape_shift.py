"""Static source contracts for bounded off-ball support/retreat behavior."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1] / "Source/SportsArcade"
H = ROOT / "Public/SportsArcadeTeamAIController.h"
CPP = ROOT / "Private/SportsArcadeTeamAIController.cpp"


class TeamAIShapeShiftTests(unittest.TestCase):
    def test_support_and_retreat_tunable_in_editor(self):
        header = H.read_text(encoding="utf-8")
        self.assertIn("AttackingSupportShiftCm = 220.f", header)
        self.assertIn("DefensiveRecoveryShiftCm = 150.f", header)
        self.assertIn('Category="Sports Arcade|AI|Shape"', header)

    def test_lateral_support_responds_to_nearby_opponent(self):
        header = H.read_text(encoding="utf-8")
        code = CPP.read_text(encoding="utf-8")
        self.assertIn("PassingSupportLateralShiftCm = 350.f", header)
        self.assertIn("PassingSupportOpponentSearchCm = 1000.f", header)
        section = code.split("else if (IsValid(Ball))", 1)[1].split(
            "// Only the closest eligible outfielder presses", 1
        )[0]
        self.assertIn("RecentTouchTeam == Footballer->TeamIndex", section)
        self.assertIn("NearestOpponent", section)
        self.assertIn("Opp->GetWorld() != GetWorld()", section)
        self.assertIn("Opp->GetActorLocation().ContainsNaN()", section)
        self.assertIn("FMath::Clamp(PassingSupportLateralShiftCm, 0.f, 1200.f)", section)

    def test_last_touch_drives_small_team_shape_changes_not_pressing(self):
        code = CPP.read_text(encoding="utf-8")
        section = code.split("else if (IsValid(Ball))", 1)[1].split(
            "if (Nearest == Footballer", 1)[0]
        self.assertIn("GetLastTouchTeamIndex()", section)
        self.assertIn("RecentTouchTeam == Footballer->TeamIndex", section)
        self.assertIn("RecentTouchTeam == 1 - Footballer->TeamIndex", section)
        self.assertIn("FMath::Clamp(AttackingSupportShiftCm, 0.f, 500.f)", section)
        self.assertIn("FMath::Clamp(DefensiveRecoveryShiftCm, 0.f, 500.f)", section)
        self.assertLess(section.index("GetLastTouchTeamIndex()"),
                        section.index("for (ASportsArcadeCharacter* Candidate"))


if __name__ == "__main__":
    unittest.main()
