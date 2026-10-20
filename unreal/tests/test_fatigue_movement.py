"""Source checks for stamina-sensitive movement and jockey stopping."""
from pathlib import Path
import unittest

SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeCharacter.cpp"

class FatigueMovementTests(unittest.TestCase):
    def test_fatigue_scales_speed_and_acceleration_with_clamped_floor(self):
        code = SOURCE.read_text(encoding="utf-8")
        self.assertIn("const float StaminaFraction = CurrentStamina / Capacity", code)
        self.assertIn("FMath::Lerp(0.72f, 1.f", code)
        self.assertIn("FMath::Clamp(StaminaFraction / 0.4f, 0.f, 1.f)", code)
        self.assertIn("PaceFactor * FatigueSpeedFactor", code)
        self.assertIn("1600.f * AccelerationFactor * FatigueSpeedFactor", code)

    def test_sprint_release_respects_fatigue_and_jockey(self):
        code = SOURCE.read_text(encoding="utf-8")
        block = code.split("void ASportsArcadeCharacter::EndSprint()", 1)[1].split(
            "float ASportsArcadeCharacter::GetStaminaNormalized()", 1)[0]
        self.assertIn("CurrentStamina / (Capacity * 0.4f)", block)
        self.assertIn("(bJockeyHeld && bPlayActive ? JockeySpeed : JogSpeed)", block)
        self.assertIn("PaceFactor * FatigueSpeedFactor", block)

    def test_jockey_brakes_more_sharply_without_changing_regular_braking(self):
        code = SOURCE.read_text(encoding="utf-8")
        self.assertIn("bJockeyHeld && bPlayActive ? 2800.f : 2100.f", code)
        self.assertIn("bJockeyHeld && bPlayActive ? JockeySpeed", code)

if __name__ == "__main__":
    unittest.main()
