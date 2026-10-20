"""Source contract checks for provisional advanced-player attacking runs.

These checks do not compile or simulate Unreal Engine.
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1] / "Source/SportsArcade"
HEADER = ROOT / "Public/SportsArcadeTeamAIController.h"
SOURCE = ROOT / "Private/SportsArcadeTeamAIController.cpp"

class ForwardRunTests(unittest.TestCase):
    def test_editor_adjustable_run_limit(self):
        self.assertIn("ForwardRunOffsetCm = 650.f", HEADER.read_text(encoding="utf-8"))
        self.assertIn("ForwardRunLaneSeparationCm = 250.f", HEADER.read_text(encoding="utf-8"))

    def test_runs_require_attributed_attack_and_forward_slots(self):
        code = SOURCE.read_text(encoding="utf-8")
        section = code.split("// Forward players make a modest run", 1)[1].split(
            "// Hold the authored back line", 1
        )[0]
        for contract in (
            "RecentTouchTeam == Footballer->TeamIndex",
            "Slot >= 9 && Slot <= 10",
            "Ball->GetActorLocation().X * AttackSign > 0.f",
            "FMath::Clamp(ForwardRunOffsetCm, 0.f, 1500.f)",
            "Slot == 9 ? -1.f : 1.f",
            "FMath::Clamp(ForwardRunLaneSeparationCm, 0.f, 800.f)",
        ):
            self.assertIn(contract, section)

if __name__ == "__main__":
    unittest.main()
