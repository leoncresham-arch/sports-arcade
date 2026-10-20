"""Static regression checks for valid football contact positions."""
from pathlib import Path
import unittest

SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeBall.cpp"

class BallContactPositionIntegrityTests(unittest.TestCase):
    def test_kick_requires_finite_player_and_ball_location(self):
        code = SOURCE.read_text(encoding="utf-8")
        part = code.split("bool ASportsArcadeBall::KickByPlayer(", 1)[1].split(
            "void ASportsArcadeBall::StopMotion()", 1)[0]
        self.assertIn("Player->GetActorLocation().ContainsNaN()", part)
        self.assertIn("GetActorLocation().ContainsNaN()", part)
        self.assertLess(part.index("ContainsNaN()"), part.index("LastTouchPlayer = Player"))

    def test_referee_placement_rejects_nonfinite_mark(self):
        code = SOURCE.read_text(encoding="utf-8")
        body = code.split("void ASportsArcadeBall::PlaceAtRest(", 1)[1].split(
            "void ASportsArcadeBall::ResetToCentre()", 1)[0]
        self.assertIn("WorldPosition.ContainsNaN()", body)
        self.assertLess(body.index("WorldPosition.ContainsNaN()"), body.index("SetActorLocation("))

    def test_reset_ignores_invalid_initial_location(self):
        code = SOURCE.read_text(encoding="utf-8")
        body = code.split("void ASportsArcadeBall::ResetToCentre()", 1)[1].split(
            "bool ASportsArcadeBall::ApplyControlledTouch(", 1)[0]
        self.assertIn("InitialSpawnLocation.ContainsNaN()", body)
        self.assertLess(body.index("InitialSpawnLocation.ContainsNaN()"), body.index("SetActorLocation("))

    def test_controlled_touch_rejects_bad_velocity(self):
        code = SOURCE.read_text(encoding="utf-8")
        part = code.split("bool ASportsArcadeBall::ApplyControlledTouch(", 1)[1].split(
            "int32 ASportsArcadeBall::GetLastTouchTeamIndex()", 1)[0]
        self.assertIn("CollisionSphere->GetPhysicsLinearVelocity().ContainsNaN()", part)
        self.assertIn("Player->GetActorLocation().ContainsNaN()", part)
        self.assertIn("GetActorLocation().ContainsNaN()", part)
        self.assertLess(part.index("ContainsNaN()"), part.index("LastTouchPlayer = Player"))

if __name__ == "__main__":
    unittest.main()
