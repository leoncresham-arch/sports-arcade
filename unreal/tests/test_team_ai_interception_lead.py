"""Source contract for distance-aware AI ball interception."""
from pathlib import Path
import unittest

SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp"

class InterceptionLeadTests(unittest.TestCase):
    def test_presser_adapts_prediction_to_separation(self):
        code = SOURCE.read_text(encoding="utf-8")
        part = code.split("Nearest == Footballer &&", 1)[1].split(
            "// No controlled kick attempts", 1)[0]
        self.assertIn("FMath::Sqrt(BestDistance)", part)
        header = SOURCE.parents[1] / "Public/SportsArcadeTeamAIController.h"
        self.assertIn("PresserTravelSpeedCmPerSec = 500.f", header.read_text(encoding="utf-8"))
        self.assertIn("SeparationCm / FMath::Clamp(PresserTravelSpeedCmPerSec, 100.f, 1000.f)", part)
        self.assertIn("FMath::Clamp(BallPredictionSeconds, 0.f, 0.5f)", part)
        self.assertIn("BallVelocity * GroundLeadSeconds", part)
        self.assertIn("BallVelocity.Z > 100.f ? 0.f : FMath::Min(LeadSeconds, 0.12f)", part)
        self.assertIn("Footballer->GetActorLocation().Z) > 150.f", part)
        self.assertIn("if (Predicted.ContainsNaN()) { StopMovement(); return; }", part)
        self.assertIn("Ball->GetVelocity().GetClampedToMaxSize(3500.f)", part)
        self.assertIn("-HalfLength + 100.f, HalfLength - 100.f", part)
        self.assertIn("-HalfWidth + 100.f, HalfWidth - 100.f", part)
        self.assertIn("Ball->GetVelocity().ContainsNaN()", part)

if __name__ == "__main__":
    unittest.main()
