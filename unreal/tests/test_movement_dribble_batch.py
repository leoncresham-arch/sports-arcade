"""Static source contracts for the integrated locomotion and controlled-touch batch."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1] / "Source/SportsArcade"
CHARACTER = (ROOT / "Private/SportsArcadeCharacter.cpp").read_text(encoding="utf-8")
HEADER = (ROOT / "Public/SportsArcadeCharacter.h").read_text(encoding="utf-8")

class MovementDribbleBatchTests(unittest.TestCase):
    def test_turn_braking_responds_to_opposed_travel_and_input(self):
        self.assertIn("FVector::DotProduct(TravelDirection, IntendedDirection) < 0.f", CHARACTER)
        self.assertIn("bHardTurn ? FMath::Clamp(TurnBrakingWeight, 0.f, 1.f)", CHARACTER)
        self.assertIn("float TurnBrakingWeight = 0.35f", HEADER)

    def test_fast_receding_balls_cannot_be_recaptured_automatically(self):
        self.assertIn("RelativeBallTravel.SizeSquared2D() > FMath::Square(1150.f)", CHARACTER)
        self.assertIn("BallTravel.ContainsNaN()", CHARACTER)
        self.assertIn("RelativeBallTravel.GetSafeNormal2D()", CHARACTER)

    def test_controlled_touch_scales_with_running_speed(self):
        self.assertIn("const float TouchSpeedFactor = FMath::Lerp(0.65f, 1.15f", CHARACTER)
        self.assertIn("TravelSpeed / FMath::Max(SprintSpeed, 1.f)", CHARACTER)
        self.assertIn("RunningDribbleTouchScale", CHARACTER)
        self.assertIn("float RunningDribbleTouchScale = 1.f", HEADER)

if __name__ == "__main__":
    unittest.main()
