"""Static safety contracts for native ball control query; not a UE runtime test."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1] / "Source/SportsArcade"
CPP = (ROOT / "Private/SportsArcadeBall.cpp").read_text()
HEADER = (ROOT / "Public/SportsArcadeBall.h").read_text()

class LiveBallControlTests(unittest.TestCase):
    def test_exposes_live_control_separately_from_history(self):
        self.assertIn("GetControlledPlayer(", HEADER)
        self.assertIn("GetLastTouchPlayer()", HEADER)
        self.assertIn("GetLastTouchTeamIndex()", HEADER)
    def test_control_denied_without_simulated_finite_physics(self):
        part = CPP.split("ASportsArcadeCharacter* ASportsArcadeBall::GetControlledPlayer(", 1)[1]
        self.assertIn("!CollisionSphere->IsSimulatingPhysics()", part)
        self.assertIn("GetPhysicsLinearVelocity().ContainsNaN()", part)
        self.assertIn("Player->GetWorld() != GetWorld()", part)
        self.assertIn("MaximumPlanarRangeCm <= 0.f", part)
        self.assertIn("MaximumHeightDifferenceCm <= 0.f", part)
        self.assertIn("FVector::DistSquared2D(Player->GetActorLocation()", part)
    def test_touch_changes_attribution_only_after_valid_impulse(self):
        part = CPP.split("bool ASportsArcadeBall::ApplyControlledTouch(", 1)[1].split(
            "int32 ASportsArcadeBall::GetLastTouchTeamIndex()", 1)[0]
        self.assertLess(part.index("TouchDelta.SizeSquared2D() < FMath::Square(1.f)"),
                        part.index("LastTouchPlayer = Player;"))

if __name__ == "__main__":
    unittest.main()
