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

    def test_keyboard_manual_restart_uses_awarded_taker(self):
        ini = read("Config/DefaultInput.ini")
        header = read("Source/SportsArcade/Public/SportsArcadeCharacter.h")
        cpp = read("Source/SportsArcade/Private/SportsArcadeCharacter.cpp")
        self.assertIn('ActionName="TakeManualRestart",Key=M', ini)
        self.assertIn("void TakeStagedRestart();", header)
        self.assertIn('BindAction(TEXT("TakeManualRestart")', cpp)
        self.assertIn("Mode->CanTakeManualRestart()", cpp)
        self.assertIn("Mode->PendingRestartTaker->GetActorForwardVector()", cpp)
        self.assertIn("Mode->TakeManualRestart(", cpp)

    def test_debug_referee_award_keyboard_routes_to_game_mode(self):
        ini = read("Config/DefaultInput.ini")
        header = read("Source/SportsArcade/Public/SportsArcadeCharacter.h")
        cpp = read("Source/SportsArcade/Private/SportsArcadeCharacter.cpp")
        for action, key, handler, team in (
            ("AwardRestartHome", "F5", "AwardRestartToHome", 0),
            ("AwardRestartAway", "F6", "AwardRestartToAway", 1),
        ):
            self.assertIn(f'ActionName="{action}",Key={key}', ini)
            self.assertIn(f"void {handler}();", header)
            self.assertIn(f'BindAction(TEXT("{action}")', cpp)
            self.assertIn(f"void ASportsArcadeCharacter::{handler}()", cpp)
            self.assertIn(f"Mode->AwardUndeterminedRestart({team});", cpp)

    def test_keyboard_physical_shootout_uses_staged_taker(self):
        ini = read("Config/DefaultInput.ini")
        header = read("Source/SportsArcade/Public/SportsArcadeCharacter.h")
        cpp = read("Source/SportsArcade/Private/SportsArcadeCharacter.cpp")
        for action, key, handler in (
            ("StageCupPenalty", "P", "StageShootoutPenalty"),
            ("KickCupPenalty", "O", "KickShootoutPenalty"),
        ):
            self.assertIn(f'ActionName="{action}",Key={key}', ini)
            self.assertIn(f"void {handler}();", header)
            self.assertIn(f'BindAction(TEXT("{action}")', cpp)
        self.assertIn("Mode->CanStageCupPenaltyKick()", cpp)
        self.assertIn("Mode->GetStagedCupPenaltyTaker()", cpp)
        self.assertIn("Mode->KickStagedCupPenalty(Taker->GetActorForwardVector())", cpp)

    def test_keyboard_referee_scores_physical_penalty(self):
        ini = read("Config/DefaultInput.ini")
        header = read("Source/SportsArcade/Public/SportsArcadeCharacter.h")
        cpp = read("Source/SportsArcade/Private/SportsArcadeCharacter.cpp")
        for action, key, handler, result in (
            ("PenaltyGoalReview", "F7", "AwardPenaltyGoal", "true"),
            ("PenaltyMissReview", "F8", "AwardPenaltyMiss", "false"),
        ):
            self.assertIn(f'ActionName="{action}",Key={key}', ini)
            self.assertIn(f"void {handler}();", header)
            self.assertIn(f'BindAction(TEXT("{action}")', cpp)
            self.assertIn(f"Mode->ResolvePhysicalCupPenalty({result});", cpp)

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
        self.assertEqual(cpp.count("OnPlayResumed.Broadcast();"), 2)
        self.assertIn("bool ASportsArcadeGameMode::TakeManualRestart(", cpp)
        self.assertIn("StageKickoff(1);", cpp)

    def test_exclusive_phase_query_for_match_ui(self):
        header = read("Source/SportsArcade/Public/SportsArcadeGameMode.h")
        mode = read("Source/SportsArcade/Private/SportsArcadeGameMode.cpp")
        self.assertIn("enum class ESportsArcadeMatchPhase", header)
        for phase in ("Kickoff", "Playing", "Stoppage", "HalfTime", "FullTime"):
            self.assertIn("ESportsArcadeMatchPhase::" + phase, mode)
        self.assertIn("GetCurrentMatchPhase() const", header)
        self.assertIn("PendingRestartType == ESportsArcadeRestartType::Kickoff", mode)

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
