"""Static regression checks for team AI input validity and goalkeeper identification.

These checks do not compile Unreal C++ or run navigation in the engine.
"""
from pathlib import Path
import unittest

SRC = (Path(__file__).resolve().parents[1] /
       "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp")


class TeamAIBallIntegrityTests(unittest.TestCase):
    def test_bad_ball_telemetry_stops_navigation_before_field_player_logic(self):
        code = SRC.read_text(encoding="utf-8")
        roster = code.index("if (!IsValid(Roster))")
        guard = code.index("Ball->GetActorLocation().ContainsNaN()", roster)
        selection = code.index("const int32 MembershipIndex", roster)
        self.assertLess(roster, guard)
        self.assertLess(guard, selection)
        self.assertIn("Ball->GetVelocity().ContainsNaN()", code[guard:selection])
        self.assertIn("StopMovement();", code[guard:selection])

    def test_opponents_have_valid_world_before_scoring_pass_and_marking(self):
        code = SRC.read_text(encoding="utf-8")
        self.assertIn("Opp->GetWorld() != GetWorld()", code)
        self.assertIn("It->GetWorld() != GetWorld()", code)
        self.assertIn("Teammate->GetActorLocation().ContainsNaN()", code)
        self.assertIn("Opp->GetActorLocation().ContainsNaN()", code)
        self.assertIn("It->GetActorLocation().ContainsNaN()", code)

    def test_pressing_excludes_goalkeeper_using_correct_roster_slot_mode(self):
        code = SRC.read_text(encoding="utf-8")
        section = code.split("for (ASportsArcadeCharacter* Candidate : Roster->Players)", 1)[1].split(
            "if (Nearest == Footballer", 1
        )[0]
        self.assertIn("Roster->bUseExplicitLineupSlots ? Candidate->LineupSlot == 0", section)
        self.assertIn("Roster->Players.IndexOfByKey(Candidate) == 0", section)
        self.assertIn("Candidate->GetController()", section)


if __name__ == "__main__":
    unittest.main()
