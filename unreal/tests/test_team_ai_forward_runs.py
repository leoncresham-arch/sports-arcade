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

    def test_forward_run_waits_during_fast_lateral_switch(self):
        code = SOURCE.read_text(encoding="utf-8")
        section = code.split("// Forward players make a modest run", 1)[1].split(
            "// Central midfielders supply", 1
        )[0]
        self.assertIn("FMath::Abs(Ball->GetVelocity().Y) <= 1100.f", section)

    def test_forward_run_waits_during_fast_backward_pass(self):
        code = SOURCE.read_text(encoding="utf-8")
        section = code.split("// Forward players make a modest run", 1)[1].split(
            "// Central midfielders supply", 1
        )[0]
        self.assertIn("Ball->GetVelocity().X * AttackSign >= -450.f", section)
        self.assertIn("!Ball->GetVelocity().ContainsNaN()", section)

    def test_midfield_support_requires_receivable_ball_height(self):
        code = SOURCE.read_text(encoding="utf-8")
        section = code.split("// Central midfielders supply", 1)[1].split(
            "// Suppress advancing support", 1
        )[0]
        self.assertIn("Slot >= 5 && Slot <= 8", section)
        self.assertIn("Footballer->GetActorLocation().Z) <= 160.f", section)

    def test_aerial_clearance_does_not_trigger_grounded_forward_burst(self):
        code = SOURCE.read_text(encoding="utf-8")
        section = code.split("// Forward players make a modest run", 1)[1].split(
            "// Central midfielders supply", 1
        )[0]
        self.assertIn("Footballer->GetActorLocation().Z) <= 160.f", section)

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
