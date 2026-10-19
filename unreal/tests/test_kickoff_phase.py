"""Source-level regression coverage for the provisional kickoff staging path.

These guards are not a substitute for UE 5.5 compilation or playtesting.
"""
import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "Source/SportsArcade/Private/SportsArcadeGameMode.cpp").read_text()
HEADER = (ROOT / "Source/SportsArcade/Public/SportsArcadeGameMode.h").read_text()


class KickoffRestartContract(unittest.TestCase):
    def test_typed_kickoff_is_exposed(self):
        self.assertIn("GoalKick, Kickoff, Undetermined", HEADER)
        self.assertIn("void StageKickoff(int32 TakingTeam);", HEADER)

    def test_both_scoring_directions_stage_other_team(self):
        for team in (0, 1):
            self.assertRegex(SOURCE, rf"RecordGoal\({'true' if team == 1 else 'false'}\);\s*StageKickoff\({team}\);")

    def test_half_time_requires_second_half_kickoff(self):
        method = SOURCE.split("void ASportsArcadeGameMode::ResumeSecondHalf()", 1)[1].split("void ASportsArcadeGameMode::ResetMatch()", 1)[0]
        self.assertIn("StageKickoff(1);", method)
        self.assertNotIn("OnPlayResumed.Broadcast();", method)

    def test_kickoff_is_a_stoppage_with_assigned_team_and_taker(self):
        method = SOURCE.split("void ASportsArcadeGameMode::StageKickoff(", 1)[1].split("void ASportsArcadeGameMode::ClearPendingRestart()", 1)[0]
        self.assertIn("PendingRestartTeam = TakingTeam;", method)
        self.assertIn("PendingRestartTaker = FindRestartTaker(", method)
        self.assertIn("bBallOutOfPlay = true;", method)
        self.assertIn("MatchBall->ResetToCentre();", method)

    def test_restart_kick_uses_team_direction(self):
        self.assertIn("PendingRestartType == ESportsArcadeRestartType::Kickoff", SOURCE)
        self.assertIn("FVector(PendingRestartTeam == 0 ? 1.f : -1.f, 0.f, 0.08f)", SOURCE)


if __name__ == "__main__":
    unittest.main()
