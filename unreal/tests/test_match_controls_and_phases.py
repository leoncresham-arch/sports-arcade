"""Source integration checks only; these do NOT compile or execute Unreal Engine."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

def read(relative):
    return (ROOT / relative).read_text(encoding="utf-8")

class MatchControlsAndPhasesTest(unittest.TestCase):
    def test_input_bindings_and_handlers(self):
        ini = read("Config/DefaultInput.ini")
        header = read("Source/SportsArcade/Public/SportsArcadeCharacter.h")
        cpp = read("Source/SportsArcade/Private/SportsArcadeCharacter.cpp")
        for name, key, handler in (
            ("ResumeSecondHalf", "H", "ResumeMatchAfterHalfTime"),
            ("RestartMatch", "R", "RestartExhibitionMatch"),
        ):
            self.assertIn(f'ActionName="{name}",Key={key}', ini)
            self.assertIn(f'BindAction(TEXT("{name}")', cpp)
            self.assertIn(f'void {handler}();', header)
            self.assertIn(f'void ASportsArcadeCharacter::{handler}()', cpp)
        self.assertIn("Mode->ResumeSecondHalf();", cpp)
        self.assertIn("Mode->ResetMatch();", cpp)

    def test_successful_shots_increment_team_stats(self):
        header = read("Source/SportsArcade/Public/SportsArcadeGameMode.h")
        game = read("Source/SportsArcade/Private/SportsArcadeGameMode.cpp")
        player = read("Source/SportsArcade/Private/SportsArcadeCharacter.cpp")
        for name in ("HomeShots", "AwayShots", "RecordShotAttempt"):
            self.assertIn(name, header)
            self.assertIn(name, game)
        self.assertIn("if (bIsShot && IsValid(MatchMode)) MatchMode->RecordShotAttempt(TeamIndex);", player)
        self.assertIn("if (ShootingTeam == 0) ++HomeShots;", game)
        self.assertIn("else if (ShootingTeam == 1) ++AwayShots;", game)

    def test_exhibition_reset_restores_player_positions(self):
        player = read("Source/SportsArcade/Private/SportsArcadeCharacter.cpp")
        mode = read("Source/SportsArcade/Private/SportsArcadeGameMode.cpp")
        self.assertIn("InitialKickoffTransform = GetActorTransform();", player)
        self.assertIn("void ASportsArcadeCharacter::ResetToKickoffPosition()", player)
        self.assertIn("GetCharacterMovement()->StopMovementImmediately();", player)
        self.assertIn("It->ResetToKickoffPosition();", mode)

    def test_reset_and_resume_signals(self):
        header = read("Source/SportsArcade/Public/SportsArcadeGameMode.h")
        cpp = read("Source/SportsArcade/Private/SportsArcadeGameMode.cpp")
        self.assertIn("FSportsArcadeMatchPhaseEvent OnMatchReset;", header)
        self.assertIn("FSportsArcadeMatchPhaseEvent OnPlayResumed;", header)
        self.assertIn("OnMatchReset.Broadcast();", cpp)
        self.assertEqual(cpp.count("OnPlayResumed.Broadcast();"), 1)
        self.assertIn("StageKickoff(1);", cpp)

    def test_ball_settles_at_match_boundaries(self):
        mode = read("Source/SportsArcade/Private/SportsArcadeGameMode.cpp")
        ball_h = read("Source/SportsArcade/Public/SportsArcadeBall.h")
        ball_cpp = read("Source/SportsArcade/Private/SportsArcadeBall.cpp")
        self.assertIn("MatchBall->ResetToCentre();", mode)
        self.assertIn("MatchBall->StopMotion();", mode)
        self.assertIn("void StopMotion();", ball_h)
        self.assertIn("void ASportsArcadeBall::StopMotion()", ball_cpp)
        self.assertIn("SetPhysicsLinearVelocity(FVector::ZeroVector)", ball_cpp)
        self.assertIn("SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector)", ball_cpp)

if __name__ == "__main__":
    unittest.main()
