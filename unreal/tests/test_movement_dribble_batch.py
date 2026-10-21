"""Static source contracts for the integrated locomotion and controlled-touch batch."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1] / "Source/SportsArcade"
CHARACTER = (ROOT / "Private/SportsArcadeCharacter.cpp").read_text(encoding="utf-8")
HEADER = (ROOT / "Public/SportsArcadeCharacter.h").read_text(encoding="utf-8")
ATTRIBUTES = (ROOT / "Public/SportsArcadePlayerAttributes.h").read_text(encoding="utf-8")

class MovementDribbleBatchTests(unittest.TestCase):
    def test_shielding_contests_marginal_opponent_control_without_seizing_clear_winner(self):
        self.assertIn("bShieldHeld ? 25.f : 0.f", CHARACTER)
        self.assertIn("bOpponentClearlyCloser", CHARACTER)
        self.assertIn("OppToBall.SizeSquared2D() < FMath::Square(95.f)", CHARACTER)

    def test_manual_shielding_is_input_driven_and_caps_pace(self):
        self.assertIn("bool IsShieldingBall() const", HEADER)
        self.assertIn("void ASportsArcadeCharacter::BeginShield()", CHARACTER)
        self.assertIn("void ASportsArcadeCharacter::EndShield()", CHARACTER)
        self.assertIn('BindAction(TEXT("Shield"), IE_Pressed', CHARACTER)
        self.assertIn('BindAction(TEXT("Shield"), IE_Released', CHARACTER)
        self.assertIn("!bShieldHeld && bSprintHeld", CHARACTER)
        self.assertIn("bEffectiveShield ? 0.72f : 1.f", CHARACTER)
        self.assertIn("bPlayActive && bShieldHeld && bMoving && CurrentStamina > 0.f", CHARACTER)
        self.assertIn("ShieldStaminaDrainPerSecond, 0.f) * DeltaSeconds", CHARACTER)
        self.assertIn("ShieldStaminaDrainPerSecond = 4.f", HEADER)
        self.assertIn("bEffectiveShield = bShieldHeld && bPlayActive && CurrentStamina > 0.f", CHARACTER)
        self.assertIn("FatigueSpeedFactor * ShieldAccelerationFactor", CHARACTER)
        self.assertIn("((bJockeyHeld || bShieldHeld) && bPlayActive ? 2800.f : 2100.f)", CHARACTER)
        self.assertIn("(bShieldControl ? 0.55f : 1.f)", CHARACTER)
        self.assertIn("DribbleTouchInterval * 0.70f", CHARACTER)
        self.assertIn("bShieldControl = bShieldHeld && CurrentStamina > 0.f", CHARACTER)
        self.assertIn("TouchCooldownRemaining = TouchInterval", CHARACTER)
        self.assertIn("bShieldHeld = false", CHARACTER)

    def test_sprint_pivot_penalty_is_bounded_and_agility_sensitive(self):
        self.assertIn("const float SprintTurnAngle", CHARACTER)
        self.assertIn("const float TurnPaceLoss", CHARACTER)
        self.assertIn("const float AgilityTurnFactor", CHARACTER)
        self.assertIn("SprintSpeed * SprintTurnPace", CHARACTER)
        self.assertIn("GetVelocity().Size2D() / 250.f", CHARACTER)
        self.assertIn("TurnPaceLoss * SprintMomentum", CHARACTER)
        self.assertIn("0.72f, 0.9f", CHARACTER)

    def test_agility_rating_modulates_turn_speed_with_fatigue(self):
        self.assertIn("int32 Agility = 65", ATTRIBUTES)
        self.assertIn("RatingFactor(PlayerAttributes->Agility)", CHARACTER)
        self.assertIn("540.f * AgilityFactor * FatigueSpeedFactor", CHARACTER)
        self.assertIn("260.f, 760.f", CHARACTER)

    def test_turn_braking_responds_to_opposed_travel_and_input(self):
        self.assertIn("FVector::DotProduct(TravelDirection, IntendedDirection)", CHARACTER)
        self.assertIn("TurnResistance = FMath::Clamp((0.5f - TurnAlignment) / 1.5f", CHARACTER)
        self.assertIn("1200.f * TurnResistance * TurnSpeedFactor", CHARACTER)
        self.assertIn("GetVelocity().Size2D() / 350.f", CHARACTER)
        self.assertIn("float TurnBrakingWeight = 0.35f", HEADER)

    def test_dribble_touches_blend_forward_facing_with_travel(self):
        self.assertIn("TravelDir * TravelWeight", CHARACTER)
        self.assertIn("FacingDir * (1.f - TravelWeight)", CHARACTER)
        self.assertIn("bShieldControl ? 0.45f : 0.75f", CHARACTER)
        self.assertIn("FMath::Lerp(0.85f, 1.f, ControlFatigue)", CHARACTER)
        self.assertIn("CurrentStamina / ControlStaminaCapacity", CHARACTER)
        self.assertIn("!BlendedTouchDir.IsNearlyZero()", CHARACTER)
        self.assertIn("ApplyControlledTouch(this, BlendedTouchDir", CHARACTER)

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
