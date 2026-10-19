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
        self.assertIn("PendingRestartTeam = (LastTeam == 0 || LastTeam == 1) ? 1 - LastTeam : INDEX_NONE", cpp)
        self.assertIn("const int32 DefendingTeam = SideX > 0.f ? 1 : 0;", cpp)
        self.assertIn("ClearPendingRestart();", cpp)

    def test_configurable_field_geometry(self):
        header = read("Source/SportsArcade/Public/SportsArcadeGameMode.h")
        cpp = read("Source/SportsArcade/Private/SportsArcadeGameMode.cpp")
        for field in ("PitchHalfLengthCm", "PitchHalfWidthCm", "GoalHalfWidthCm", "CrossbarHeightCm"):
            self.assertIn(field, header)
            self.assertIn(field, cpp)
        self.assertIn("const float EdgeX = PitchHalfLengthCm + MatchBall->BallRadiusCm;", cpp)
        self.assertIn("const float EdgeY = PitchHalfWidthCm + MatchBall->BallRadiusCm;", cpp)

    def test_diagonal_goal_requires_in_bounds_trajectory(self):
        cpp = read('Source/SportsArcade/Private/SportsArcadeGameMode.cpp')
        self.assertIn('const float TouchEdge = PitchHalfWidthCm + MatchBall->BallRadiusCm;', cpp)
        self.assertIn('if (TouchT >= 0.f && TouchT < T) return false;', cpp)

    def test_goal_exits_before_boundary_restart(self):
        cpp = read("Source/SportsArcade/Private/SportsArcadeGameMode.cpp")
        pos = cpp.index("RecordGoal(false);")
        out = cpp.index("// Full-ball boundary crossing")
        self.assertLess(pos, out)
        self.assertIn("StageKickoff(0);", cpp[pos:out])
        self.assertLess(cpp.index("// Full-ball boundary crossing"), cpp.index("ElapsedMatchSeconds = FMath::Min("))

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
