"""Source-contract checks for the first-pass referee restart classifier.
These do not compile UE or verify game rules in engine.
"""
from pathlib import Path
import unittest

BASE = Path(__file__).resolve().parents[1]
def read(path):
    return (BASE / path).read_text(encoding="utf-8")

class RestartContracts(unittest.TestCase):
    def test_types_and_team_metadata(self):
        header = read("Source/SportsArcade/Public/SportsArcadeGameMode.h")
        cpp = read("Source/SportsArcade/Private/SportsArcadeGameMode.cpp")
        for name in ("ThrowIn", "CornerKick", "GoalKick", "Undetermined"):
            self.assertIn(name, header)
            self.assertIn("ESportsArcadeRestartType::" + name, cpp)
        self.assertIn("PendingRestartTeam = LastTeam == INDEX_NONE ? INDEX_NONE : 1 - LastTeam", cpp)
        self.assertIn("const int32 DefendingTeam = SideX > 0.f ? 1 : 0;", cpp)
        self.assertIn("ClearPendingRestart();", cpp)

    def test_boundary_and_ball_placement(self):
        cpp = read("Source/SportsArcade/Private/SportsArcadeGameMode.cpp")
        ball_h = read("Source/SportsArcade/Public/SportsArcadeBall.h")
        ball_cpp = read("Source/SportsArcade/Private/SportsArcadeBall.cpp")
        self.assertIn("const bool bTouchline = Ty < Tx", cpp)
        self.assertIn("MatchBall->PlaceAtRest(PendingRestartLocation);", cpp)
        self.assertIn("void PlaceAtRest(const FVector& WorldPosition);", ball_h)
        self.assertIn("void ASportsArcadeBall::PlaceAtRest(", ball_cpp)
        self.assertIn("StopMotion();", ball_cpp)
        self.assertIn("LastTouchPlayer.Reset();", ball_cpp)
        self.assertIn("ETeleportType::TeleportPhysics", ball_cpp)

if __name__ == "__main__":
    unittest.main()
