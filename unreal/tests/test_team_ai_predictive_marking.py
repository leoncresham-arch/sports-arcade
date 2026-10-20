"""Source contracts for lead-aware defensive marking."""
from pathlib import Path
import unittest

SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp"

class PredictiveMarkingTests(unittest.TestCase):
    def test_marking_leads_opponent_without_invalid_velocity(self):
        source = SOURCE.read_text(encoding="utf-8")
        section = source.split("// A conservative defensive shadow", 1)[1].split(
            "// Never forward invalid authored formation", 1)[0]
        self.assertIn("const FVector OpponentVelocity = Opponent->GetVelocity()", section)
        self.assertIn("OpponentVelocity.ContainsNaN() ? FVector::ZeroVector", section)
        self.assertIn("OpponentVelocity.GetClampedToMaxSize2D(650.f) * 0.20f", section)
        self.assertIn("FMath::Clamp(MarkingInfluence, 0.f, 0.5f)", section)
        self.assertIn("MarkingPoint - FVector(DefenderAttackSign * GoalSideOffset", section)
        self.assertIn("0.12f, 80.f, 180.f", section)
        self.assertIn("GoalSideMarkingPoint.X, GoalSideMarkingPoint.Y", section)
        self.assertIn("MarkingDirection", section)
        self.assertIn("MarkingDirection < -900.f", section)
        self.assertIn("bDeepDefensiveThreat", section)
        self.assertIn("-HalfLength * 0.45f", section)

if __name__ == "__main__":
    unittest.main()
