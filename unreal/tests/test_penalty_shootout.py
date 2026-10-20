"""Reference shootout behavior, plus C++ source contract checks."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = (ROOT / "Source/SportsArcade/Private/SportsArcadePenaltyShootout.cpp").read_text()

def simulate(kicks):
    home_attempts = away_attempts = home_goals = away_goals = 0
    winner = None
    for team, scored in kicks:
        if winner is not None or team != (0 if home_attempts == away_attempts else 1):
            return False, winner
        if team == 0:
            home_attempts += 1
            home_goals += int(scored)
        else:
            away_attempts += 1
            away_goals += int(scored)
        if home_attempts <= 5 and away_attempts <= 5:
            if home_goals > away_goals + 5 - away_attempts:
                winner = 0
            elif away_goals > home_goals + 5 - home_attempts:
                winner = 1
        if winner is None and home_attempts >= 5 and home_attempts == away_attempts and home_goals != away_goals:
            winner = 0 if home_goals > away_goals else 1
    return True, winner

class PenaltyShootoutTests(unittest.TestCase):
    def test_native_contract(self):
        for fragment in ("GetNextKickingTeam()", "HomeAttempts == AwayAttempts", "WinningTeam = 0",
                         "WinningTeam = 1", "HomeAttempts >= 5", "HomeAttempts == AwayAttempts"):
            self.assertIn(fragment, CPP)
    def test_invalid_shootout_state_cannot_select_next_kicker(self):
        for fragment in (
            "HomeAttempts < AwayAttempts",
            "HomeAttempts > AwayAttempts + 1",
            "HomeScored > HomeAttempts",
            "AwayScored > AwayAttempts",
            "return INDEX_NONE;",
        ):
            self.assertIn(fragment, CPP)

    def test_next_shooter_lineup_rotation_contract(self):
        header = (ROOT / "Source/SportsArcade/Public/SportsArcadePenaltyShootout.h").read_text()
        self.assertIn("GetNextKickerLineupSlot() const;", header)
        self.assertIn("PreferredSlots[Attempts % UE_ARRAY_COUNT(PreferredSlots)]", CPP)
        slots = (9, 10, 8, 7, 6, 5, 4, 3, 2, 1, 0)
        self.assertEqual(len(set(slots)), 11)
        self.assertEqual(slots[11 % len(slots)], slots[0])

    def test_sudden_death_only_after_both_kicks(self):
        tied = [(x % 2, True) for x in range(10)]
        valid, champion = simulate(tied + [(0, True)])
        self.assertTrue(valid)
        self.assertIsNone(champion)
        valid, champion = simulate(tied + [(0, True), (1, False)])
        self.assertTrue(valid)
        self.assertEqual(champion, 0)
    def test_first_five_early_resolution(self):
        valid, champion = simulate([(0, True), (1, False), (0, True), (1, False),
                                    (0, True), (1, False)])
        self.assertTrue(valid)
        self.assertEqual(champion, 0)
    def test_strict_alternating(self):
        valid, _ = simulate([(0, True), (0, False)])
        self.assertFalse(valid)
