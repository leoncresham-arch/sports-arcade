"""Static regression for valid world ownership during provisional restarts."""
from pathlib import Path
import unittest

SOURCE = (Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeGameMode.cpp").read_text()


class RestartOwnershipTests(unittest.TestCase):
    def test_world_is_checked_before_teleport(self):
        method = SOURCE.split("void ASportsArcadeGameMode::ResumeAfterBallOut()", 1)[1].split("\n}", 1)[0]
        self.assertIn("PendingRestartTaker->GetWorld() != GetWorld()", method)
        self.assertLess(method.index("PendingRestartTaker->GetWorld() != GetWorld()"),
                        method.index("SetActorLocation(TakerPosition"))

    def test_referee_stoppage_only_clears_after_restart(self):
        method = SOURCE.split("void ASportsArcadeGameMode::ResumeAfterBallOut()", 1)[1].split("\n}", 1)[0]
        self.assertLess(method.index("if (!MatchBall->KickByPlayer("),
                        method.index("bBallOutOfPlay = false;"))
        self.assertLess(method.index("if (!IsValid(PendingRestartTaker) || !IsValid(MatchBall)) return;"),
                        method.index("MatchBall->PlaceAtRest(PendingRestartLocation);"))


if __name__ == "__main__":
    unittest.main()
