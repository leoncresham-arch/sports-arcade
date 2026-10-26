"""AI scheduling properties for twenty-two footballers.

Pure scheduling checks complement source contracts; no Unreal runtime claims.
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1] / "Source/SportsArcade"
SOURCE = (ROOT / "Private/SportsArcadeTeamAIController.cpp").read_text(encoding="utf-8")
HEADER = (ROOT / "Public/SportsArcadeTeamAIController.h").read_text(encoding="utf-8")


def decision_phase(slot: int, team: int, interval: float = 0.3) -> float:
    assert 0 <= slot <= 10 and team in (0, 1)
    return max(interval, 0.05) * (slot * 2 + team) / 22.0


class SchedulingTests(unittest.TestCase):
    def test_every_lineup_player_has_a_unique_phase(self):
        phases = [decision_phase(slot, team) for slot in range(11) for team in (0, 1)]
        self.assertEqual(len(phases), 22)
        self.assertEqual(len(set(phases)), 22)
        self.assertGreaterEqual(min(phases), 0)
        self.assertLess(max(phases), 0.3)

    def test_phase_stays_within_interval_even_if_authoring_is_too_short(self):
        for interval in (0, 0.01, 0.05, 0.3, 1):
            for slot in range(11):
                for team in (0, 1):
                    self.assertLess(decision_phase(slot, team, interval), max(interval, 0.05))

    def test_native_source_uses_stable_team_and_lineup_index(self):
        self.assertIn("bool bDecisionPhaseInitialized = false;", HEADER)
        self.assertIn("AssignedFootballer->LineupSlot * 2 +", SOURCE)
        self.assertIn("AssignedFootballer->TeamIndex;", SOURCE)
        self.assertIn("static_cast<float>(PhaseIndex) / 22.f", SOURCE)
        self.assertIn("if (DecisionCountdown > 0.f) return;", SOURCE)


if __name__ == "__main__":
    unittest.main()
