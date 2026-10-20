"""Source-contract checks for the defensive contest foundation.

These do not substitute for native UE 5.5 compilation or gameplay tests.
"""
from pathlib import Path
import unittest

CHARACTER = (Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeCharacter.cpp")


class StandingTackleRecoveryTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        source = CHARACTER.read_text(encoding="utf-8")
        cls.tackle = source.split("void ASportsArcadeCharacter::TackleBall()", 1)[1]

    def test_tackle_commits_recovery_before_contact_query(self):
        tackle = self.tackle
        self.assertLess(tackle.index("TackleCooldownRemaining = FMath::Max(TackleCooldownSeconds"),
                        tackle.index("for (TActorIterator<ASportsArcadeBall>"))
        self.assertIn("TackleCooldownRemaining > 0.f", tackle)

    def test_safely_selects_nearest_reachable_forward_ball(self):
        for token in ("ClosestDistanceSq = FMath::Square(150.f)",
                      "DistanceSq >= ClosestDistanceSq",
                      "FMath::Abs(Direction.Z) >= 110.f",
                      "FVector::DotProduct(Forward, Direction.GetSafeNormal2D()) <= 0.25f",
                      "BallLocation.ContainsNaN()"):
            self.assertIn(token, self.tackle)

    def test_does_not_dispossess_own_nearby_player(self):
        for token in ("LastTouch->TeamIndex == TeamIndex",
                      "TActorIterator<ASportsArcadeCharacter>",
                      "Other->TeamIndex != TeamIndex",
                      "FMath::Square(115.f)",
                      "bTeammateControllingBall"):
            self.assertIn(token, self.tackle)
        self.assertLess(self.tackle.index("if (bTeammateControllingBall) continue;"),
                        self.tackle.index("CandidateBall = *It;"))

    def test_kicks_once_with_attributed_contact(self):
        self.assertIn("CandidateBall->KickByPlayer(this, Forward,", self.tackle)
        self.assertIn("PlayerAttributes->Tackling", self.tackle)


if __name__ == "__main__":
    unittest.main()
