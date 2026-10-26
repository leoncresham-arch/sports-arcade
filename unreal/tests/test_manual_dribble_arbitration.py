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
        self.assertIn("ClosestDribbleBall->ApplyControlledTouch(this", self.tick)

    def test_dribble_requires_real_movement_and_world(self):
        self.assertIn("GetWorld() &&", self.tick)
        self.assertIn("!GetVelocity().ContainsNaN()", self.tick)
        self.assertIn("GetVelocity().SizeSquared2D() > FMath::Square(100.f)", self.tick)

    def test_nonfinite_dribble_telemetry_blocks_touch(self):
        for token in ("if (!IsValid(*It)) continue",
                      "MyLocation.ContainsNaN()",
                      "MyVelocity.ContainsNaN()",
                      "MyFacing.ContainsNaN()",
                      "MyFacing.GetSafeNormal2D().IsNearlyZero()",
                      "MyVelocity.GetSafeNormal2D()"):
            self.assertIn(token, self.tick)

    def test_automatic_touch_needs_forward_travel_alignment(self):
        self.assertIn("const FVector TravelDirection = MyVelocity.GetSafeNormal2D()", self.tick)
        self.assertIn("FVector::DotProduct(TravelDirection, ToBall.GetSafeNormal2D()) <= 0.2f) continue;", self.tick)
        self.assertIn("ClosestDribbleBall->ApplyControlledTouch(this, ShieldTouchDir *", self.tick)

    def test_nearest_eligible_ball_selected_before_touch(self):
        self.assertIn("ClosestDribbleDistanceSq", self.tick)
        self.assertIn("ContactDistanceSq >= ClosestDribbleDistanceSq", self.tick)
        self.assertIn("ClosestDribbleBall = *It", self.tick)
        self.assertIn("if (IsValid(ClosestDribbleBall) && !ShieldTouchDir.IsNearlyZero() &&", self.tick)

    def test_unreachable_ball_rejected_before_squad_scan(self):
        block = self.tick
        scan = block.index("TActorIterator<ASportsArcadeCharacter> ContactIt(GetWorld())")
        self.assertLess(block.index("ContactDistanceSq >= ClosestDribbleDistanceSq"), scan)
        self.assertLess(block.index("TravelDirection, ToBall.GetSafeNormal2D()) <= 0.2f"), scan)
        self.assertLess(block.index("RelativeBallTravel.SizeSquared2D() > FMath::Square(1150.f)"), scan)

    def test_single_contact_scan_respects_friendly_and_opposing_claims(self):
        block = self.tick
        self.assertEqual(block.count("TActorIterator<ASportsArcadeCharacter> ContactIt(GetWorld())"), 1)
        for token in (
            "Nearby->GetWorld() != GetWorld()",
            "Nearby->TeamIndex == TeamIndex",
            "Nearby->TeamIndex == 1 - TeamIndex",
            "NearbyDistanceSquared < MyBallDistanceSq",
            "NearbyDistanceSquared < FMath::Square(115.f)",
            "NearbyDistanceSquared < FMath::Square(95.f)",
            "bOpponentClearlyCloser",
            "if (bFriendlyHasCloserControl || bOpponentHasCloserControl) continue",
        ):
            self.assertIn(token, block)
        self.assertLess(
            block.index("if (bFriendlyHasCloserControl || bOpponentHasCloserControl) continue"),
            block.index("ClosestDribbleBall->ApplyControlledTouch(this"),
        )

if __name__ == "__main__":
    unittest.main()
