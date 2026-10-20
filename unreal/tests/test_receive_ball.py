"""Source contracts for deliberate stationary receiving/first-touch control."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = (ROOT / "Source/SportsArcade/Private/SportsArcadeCharacter.cpp").read_text(encoding="utf-8")
HEADER = (ROOT / "Source/SportsArcade/Public/SportsArcadeCharacter.h").read_text(encoding="utf-8")
INPUT = (ROOT / "Config/DefaultInput.ini").read_text(encoding="utf-8")

class ReceiveBallTests(unittest.TestCase):
    def test_first_touch_input_is_explicit(self):
        self.assertIn('void ReceiveBall();', HEADER)
        self.assertIn('ActionName="Receive",Key=I', INPUT)
        self.assertIn('BindAction(TEXT("Receive"), IE_Pressed', CPP)

    def test_receive_stops_for_match_phase_and_cooldown(self):
        block = CPP.split("void ASportsArcadeCharacter::ReceiveBall()", 1)[1].split(
            "void ASportsArcadeCharacter::TackleBall()", 1
        )[0]
        self.assertIn("TouchCooldownRemaining > 0.f", block)
        self.assertIn("Match->bHalfTime || Match->bMatchFinished || Match->bBallOutOfPlay", block)
        self.assertIn("FMath::Abs(ToBall.Z) > 115.f", block)
        self.assertIn("FMath::Square(1800.f)", block)
        self.assertIn("bOpponentClearlyCloser", block)
        self.assertIn("bTeammateClearlyCloser", block)
        self.assertIn("BestBall->ApplyControlledTouch(this", block)
        self.assertIn("bShieldHeld ? 30.f : 65.f", block)
        self.assertIn("TouchCooldownRemaining = FMath::Max(DribbleTouchInterval, 0.12f)", block)

if __name__ == "__main__":
    unittest.main()
