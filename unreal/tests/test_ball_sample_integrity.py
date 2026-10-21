"""Static contract for safe native match ball position sampling."""
from pathlib import Path
import unittest

SOURCE = (Path(__file__).resolve().parents[1] / "Source" / "SportsArcade" /
          "Private" / "SportsArcadeGameMode.cpp").read_text(encoding="utf-8")


class MatchBallSampleIntegrityTests(unittest.TestCase):
    def test_nonfinite_samples_reset_crossing_history(self):
        tick = SOURCE.split("void ASportsArcadeGameMode::Tick(float DeltaSeconds)", 1)[1]
        sample = tick.split("const FVector Position = MatchBall->GetActorLocation();", 1)[1]
        self.assertIn("if (Position.ContainsNaN())", sample)
        self.assertIn("bHasPreviousBallPosition = false;", sample)
        self.assertIn("PreviousBallPosition.ContainsNaN()", sample)
        self.assertLess(sample.index("PreviousBallPosition.ContainsNaN()"),
                        sample.index("const FVector Travel = Position - PreviousBallPosition;"))


if __name__ == "__main__":
    unittest.main()
