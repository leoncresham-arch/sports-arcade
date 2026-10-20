"""Source-level contracts for bounded physical shootout keeper anticipation.

This does not simulate navmesh or compile Unreal Engine.
"""
from pathlib import Path
import unittest

SOURCE = (Path(__file__).resolve().parents[1] /
          "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp")


class PenaltyKeeperPredictionTests(unittest.TestCase):
    def test_keeper_tracks_projected_ball_arrival(self):
        code = SOURCE.read_text(encoding="utf-8")
        penalty = code.split("if (IsValid(Match) && Match->IsCupPenaltyAwaitingResult())", 1)[1].split(
            "if (IsValid(Match) && (Match->bHalfTime", 1
        )[0]
        self.assertIn("(KeeperPlaneX - BallPosition.X) / BallVelocity.X", penalty)
        self.assertIn("TravelSeconds >= 0.f && TravelSeconds <= 1.5f", penalty)
        self.assertIn("BallPosition.Y + BallVelocity.Y * TravelSeconds", penalty)
        self.assertIn("FMath::Clamp(AnticipatedY, -300.f, 300.f)", penalty)
        self.assertIn("Keeper->LineupSlot != 0", penalty)
        self.assertIn("Keeper->TeamIndex != Match->GetCupPenaltyDefendingTeam()", penalty)

    def test_parry_requires_ball_ahead_and_reachable(self):
        code = SOURCE.read_text(encoding="utf-8")
        penalty = code.split("if (IsValid(Match) && Match->IsCupPenaltyAwaitingResult())", 1)[1].split(
            "if (IsValid(Match) && (Match->bHalfTime", 1
        )[0]
        self.assertIn("(BallPosition.X - KeeperPlaneX) * GoalwardSign <= 35.f", penalty)
        self.assertIn("BallPosition.Z - Keeper->GetActorLocation().Z <= 220.f", penalty)
        self.assertIn("bIncomingPenalty && bBallInFront && bReachableHeight", penalty)
        self.assertIn("BallPosition.ContainsNaN() || BallVelocity.ContainsNaN()", penalty)
        self.assertIn("Ball->KickByPlayer(Keeper", penalty)

    def test_invalid_frame_deltas_do_not_tick_ai_cooldowns(self):
        code = SOURCE.read_text(encoding="utf-8")
        before = code.split("void ASportsArcadeTeamAIController::Tick(float DeltaSeconds)", 1)[1].split(
            "ASportsArcadeGameMode* Match", 1
        )[0]
        self.assertLess(before.index("!FMath::IsFinite(DeltaSeconds)"),
                        before.index("DecisionCountdown -= DeltaSeconds"))


if __name__ == "__main__":
    unittest.main()
