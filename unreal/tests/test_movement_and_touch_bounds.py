"""Source contracts for safe player inputs and bounded grounded ball control."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1] / "Source" / "SportsArcade" / "Private"
CHARACTER = (ROOT / "SportsArcadeCharacter.cpp").read_text(encoding="utf-8")
BALL = (ROOT / "SportsArcadeBall.cpp").read_text(encoding="utf-8")


class MovementBallSafetyTests(unittest.TestCase):
    def test_sprint_momentum_limits_rotation_rate(self):
        self.assertIn("const float SprintRotationFactor = bSprinting && !bSprintExhausted", CHARACTER)
        self.assertIn("FMath::Clamp(GetVelocity().Size2D() / 650.f, 0.f, 1.f)", CHARACTER)
        self.assertIn("AgilityFactor * FatigueSpeedFactor *", CHARACTER)
        self.assertIn("SprintRotationFactor, 260.f, 760.f", CHARACTER)

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

    def test_reverse_direction_touches_respect_ball_momentum(self):
        self.assertIn("const float ReverseAlignment = FVector::DotProduct", BALL)
        self.assertIn("IncomingSpeed > 300.f", BALL)
        self.assertIn("FMath::Lerp(1.f, 0.75f, ReverseDifficulty)", BALL)
        self.assertIn("TouchDeltaCap * FMath::Lerp", BALL)

    def test_fast_incoming_ball_reduces_single_touch_correction(self):
        self.assertIn("const float ReceptionDifficulty = FMath::Clamp(", BALL)
        self.assertIn("(IncomingSpeed - 900.f) / 2100.f", BALL)
        self.assertIn("FMath::Lerp(1.f, 0.65f, ReceptionDifficulty)", BALL)
        self.assertIn("Correction.GetClampedToMaxSize(\n        TouchDeltaCap * FMath::Lerp(1.f, 0.75f, ReverseDifficulty))", BALL)

    def test_ground_touch_intent_and_impulse_are_bounded(self):
        start = BALL.index("bool ASportsArcadeBall::ApplyControlledTouch")
        body = BALL[start:]
        self.assertIn("DesiredVelocity.ContainsNaN()", body)
        self.assertIn(".GetClampedToMaxSize2D(1400.f)", body)
        self.assertIn("Correction.GetClampedToMaxSize(\n        TouchDeltaCap * FMath::Lerp(1.f, 0.75f, ReverseDifficulty))", body)
        self.assertIn("if (TouchDelta.SizeSquared2D() < FMath::Square(1.f)) return false;", body)
        self.assertLess(body.index("GetClampedToMaxSize2D(1400.f)"),
                        body.index("CollisionSphere->AddImpulse(TouchDelta * BallMassKg)"))


if __name__ == "__main__":
    unittest.main()
