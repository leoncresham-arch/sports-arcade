"""Guards for player-driven dribble arbitration; source checks only."""
from pathlib import Path
import unittest

SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeCharacter.cpp"

class ManualDribbleArbitrationTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        code = SOURCE.read_text(encoding="utf-8")
        cls.tick = code.split("void ASportsArcadeCharacter::Tick(float DeltaSeconds)", 1)[1].split(
            "void ASportsArcadeCharacter::PassBall()", 1)[0]

    def test_manual_dribble_does_not_override_ai_decisions(self):
        self.assertIn("bPlayActive && IsPlayerControlled() && !bJockeyHeld", self.tick)
        self.assertIn("It->ApplyControlledTouch(this", self.tick)

    def test_opponent_closer_to_ball_blocks_automatic_steal(self):
        for token in ("bOpponentHasCloserControl",
                      "Opponent->TeamIndex == TeamIndex",
                      "OppToBall.SizeSquared2D() < FMath::Square(95.f)",
                      "OppToBall.SizeSquared2D() < MyBallDistanceSq",
                      "if (bOpponentHasCloserControl) continue"):
            self.assertIn(token, self.tick)
        self.assertLess(self.tick.index("if (bOpponentHasCloserControl) continue"),
                        self.tick.index("It->ApplyControlledTouch(this"))

    def test_friendly_proximity_arbitrates_controlled_touches(self):
        block = self.tick
        for check in ("TActorIterator<ASportsArcadeCharacter> MateIt(GetWorld())",
                      "Mate->TeamIndex != TeamIndex",
                      "MateToBall.SizeSquared2D() < MyBallDistanceSq",
                      "FMath::Abs(MateToBall.Z) <= 115.f",
                      "if (bFriendlyHasCloserControl) continue",
                      "BallLocation.ContainsNaN()"):
            self.assertIn(check, block)
        self.assertLess(block.index("if (bFriendlyHasCloserControl) continue"),
                        block.index("It->ApplyControlledTouch(this"))

if __name__ == "__main__":
    unittest.main()
