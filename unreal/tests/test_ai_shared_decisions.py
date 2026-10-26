"""Regression contracts for the shared opponent snapshot used by 11v11 decisions.

These are source-structure checks, not a substitute for Unreal runtime tests.
"""
from pathlib import Path
import re
import unittest

SOURCE = (Path(__file__).resolve().parents[1] /
          "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp")


class SharedOpponentDecisionTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.code = SOURCE.read_text(encoding="utf-8")

    def test_one_snapshot_and_reused_decision_paths(self):
        code = self.code
        self.assertEqual(len(re.findall(
            r"for \(TActorIterator<ASportsArcadeCharacter> OpponentIt\(GetWorld\(\)\)",
            code)), 1)
        self.assertIn("TArray<ASportsArcadeCharacter*> TacticalOpponents;", code)
        for name, minimum in (("Opp", 3), ("Threat", 1), ("Defender", 1)):
            self.assertGreaterEqual(len(re.findall(
                rf"for \(ASportsArcadeCharacter\* {name} : TacticalOpponents\)",
                code)), minimum)

    def test_snapshot_filters_world_team_and_invalid_position(self):
        code = self.code
        snapshot = code.split(
            "TArray<ASportsArcadeCharacter*> TacticalOpponents;", 1)[1]
        snapshot = snapshot.split("FVector Target =", 1)[0]
        self.assertIn("Candidate->GetWorld() == GetWorld()", snapshot)
        self.assertIn("Candidate->TeamIndex != Footballer->TeamIndex", snapshot)
        self.assertIn("!Candidate->GetActorLocation().ContainsNaN()", snapshot)

    def test_dead_ball_guard_precedes_snapshot(self):
        code = self.code
        self.assertLess(code.index("Match->bHalfTime || Match->bMatchFinished"),
                        code.index("TArray<ASportsArcadeCharacter*> TacticalOpponents;"))


if __name__ == "__main__":
    unittest.main()
