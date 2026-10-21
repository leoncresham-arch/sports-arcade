"""Source contracts for safe player inputs and bounded grounded ball control."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1] / "Source" / "SportsArcade" / "Private"
CHARACTER = (ROOT / "SportsArcadeCharacter.cpp").read_text(encoding="utf-8")
BALL = (ROOT / "SportsArcadeBall.cpp").read_text(encoding="utf-8")


class MovementBallSafetyTests(unittest.TestCase):
    def test_forward_and_lateral_axes_are_finite_and_clamped(self):
        for direction in ("Forward", "Right"):
            start = CHARACTER.index("void ASportsArcadeCharacter::Move" + direction)
            end = CHARACTER.index("\n}", start)
            body = CHARACTER[start:end]
            self.assertIn("FMath::IsFinite(Value)", body)
            self.assertIn("FMath::Clamp(Value, -1.f, 1.f)", body)

    def test_character_tick_ignores_nonfinite_frame_time(self):
        start = CHARACTER.index("void ASportsArcadeCharacter::Tick(float DeltaSeconds)")
        self.assertIn("if (!FMath::IsFinite(DeltaSeconds) || DeltaSeconds <= 0.f) return;",
                      CHARACTER[start:start + 250])

    def test_ground_touch_intent_and_impulse_are_bounded(self):
        start = BALL.index("bool ASportsArcadeBall::ApplyControlledTouch")
        body = BALL[start:]
        self.assertIn("DesiredVelocity.ContainsNaN()", body)
        self.assertIn(".GetClampedToMaxSize2D(1400.f)", body)
        self.assertIn("Correction.GetClampedToMaxSize(TouchDeltaCap)", body)
        self.assertIn("if (TouchDelta.SizeSquared2D() < FMath::Square(1.f)) return false;", body)
        self.assertLess(body.index("GetClampedToMaxSize2D(1400.f)"),
                        body.index("CollisionSphere->AddImpulse(TouchDelta * BallMassKg)"))


if __name__ == "__main__":
    unittest.main()
