"""Regression contracts for predicted receiver locations in AI passes."""
from pathlib import Path
import unittest

SOURCE = Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp"


class ReceiverLeadingTests(unittest.TestCase):
    def test_pass_power_tracks_receiver_distance(self):
        source = SOURCE.read_text(encoding="utf-8")
        self.assertIn("float BestPassDistanceCm = 0.f", source)
        self.assertIn("BestPassDistanceCm = Distance", source)
        self.assertIn("FMath::Clamp(900.f + BestPassDistanceCm * 0.24f, 1000.f, 1800.f)", source)
        self.assertIn("BestPassScore > -BIG_NUMBER ? PassImpulse : 1700.f", source)

    def test_moving_receivers_are_lead_and_bad_velocity_ignored(self):
        source = SOURCE.read_text(encoding="utf-8")
        passes = source.split("for (ASportsArcadeCharacter* Teammate : Roster->Players)", 1)[1].split(
            "if (Ball->KickByPlayer(Footballer", 1)[0]
        self.assertIn("ReceiverVelocity.ContainsNaN()", passes)
        self.assertIn("ReceiverVelocity.GetClampedToMaxSize2D(650.f) * 0.25f", passes)
        self.assertIn("const FVector2D End(Ball->GetActorLocation() + Delta)", passes)
        self.assertIn("Opp->GetActorLocation(), Ball->GetActorLocation() + Delta)", passes)
        self.assertIn("Direction = Delta.GetSafeNormal2D()", passes)


if __name__ == "__main__":
    unittest.main()
