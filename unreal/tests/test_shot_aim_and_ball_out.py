"""Source contract regression checks; not Unreal compilation or a referee playtest."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
def source(path):
    return (ROOT / path).read_text(encoding="utf-8")

class ShootingAndStoppageTests(unittest.TestCase):
    def test_aim_and_lift(self):
        cpp = source("Source/SportsArcade/Private/SportsArcadeCharacter.cpp")
        header = source("Source/SportsArcade/Public/SportsArcadeCharacter.h")
        self.assertIn("GetLastMovementInputVector().GetSafeNormal2D()", cpp)
        self.assertIn("bIsShot ? (Forward + FVector(0.f, 0.f, ShotLiftRatio))", cpp)
        self.assertIn("float ShotLiftRatio = 0.16f;", header)
        self.assertIn("StrikeNearbyBall(PassImpulseKgCmPerSec * Factor, false)", cpp)
        self.assertIn("StrikeNearbyBall(ShotImpulseKgCmPerSec * Factor, true)", cpp)

    def test_out_of_play(self):
        game = source("Source/SportsArcade/Private/SportsArcadeGameMode.cpp")
        header = source("Source/SportsArcade/Public/SportsArcadeGameMode.h")
        player = source("Source/SportsArcade/Private/SportsArcadeCharacter.cpp")
        config = source("Config/DefaultInput.ini")
        self.assertIn("OnBallOutOfPlay.Broadcast(Position, LastTeam)", game)
        self.assertIn("void ASportsArcadeGameMode::ResumeAfterBallOut()", game)
        self.assertIn("bBallOutOfPlay = true;", game)
        self.assertIn("FSportsArcadeBallOutEvent OnBallOutOfPlay;", header)
        self.assertIn("Mode->ResumeAfterBallOut();", player)
        self.assertIn('ActionName="RestartBallOut",Key=B', config)

if __name__ == "__main__":
    unittest.main()
