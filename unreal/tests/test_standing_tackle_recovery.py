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
        self.assertIn("CurrentStamina = FMath::Max(0.f, CurrentStamina - FMath::Max(0.f, TackleStaminaCost))", tackle)
        self.assertLess(tackle.index("CurrentStamina = FMath::Max(0.f, CurrentStamina -"),
                        tackle.index("for (TActorIterator<ASportsArcadeBall>"))

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
                      "!IsValid(Other)",
                      "Other->GetWorld() != GetWorld()",
                      "FMath::Square(115.f)",
                      "bTeammateControllingBall"):
            self.assertIn(token, self.tackle)
        self.assertLess(self.tackle.index("if (bTeammateControllingBall) continue;"),
                        self.tackle.index("CandidateBall = *It;"))

    def test_challenge_deflects_from_nearby_opponent_without_backwards_kick(self):
        for token in ("PreviousCarrier = CandidateBall->GetLastTouchPlayer()",
                      "PreviousCarrier->GetWorld() == GetWorld()",
                      "PreviousCarrier->TeamIndex != TeamIndex",
                      "FMath::Square(220.f)",
                      "Forward * 0.65f + Away * 0.35f",
                      "FVector::DotProduct(ChallengeDirection, Forward) < 0.35f",
                      "ChallengeDirection = Forward"):
            self.assertIn(token, self.tackle)

    def test_fast_receding_balls_are_out_of_standing_tackle_reach(self):
        for token in ("BallVelocity.ContainsNaN()",
                      "BallVelocity.SizeSquared2D() > FMath::Square(1200.f)",
                      "FVector2D::DotProduct(FVector2D(Direction), FVector2D(BallVelocity)) > 0.f"):
            self.assertIn(token, self.tackle)
        self.assertLess(self.tackle.index("BallVelocity.ContainsNaN()"),
                        self.tackle.index("CandidateBall = *It;"))

    def test_endurance_scales_tackle_strength(self):
        for token in ("TackleCapacity = FMath::Max(MaxStamina",
                      "FMath::Clamp(CurrentStamina, 0.f, TackleCapacity)",
                      "FMath::Lerp(0.6f, 1.f",
                      "CurrentStamina / (TackleCapacity * 0.35f)",
                      "850.f * TackleEnergyFactor *"):
            self.assertIn(token, self.tackle)
        self.assertLess(self.tackle.index("const float TackleEnergyFactor"),
                        self.tackle.index("CurrentStamina = FMath::Max(0.f, CurrentStamina -"))

    def test_successful_tackle_delays_followup_auto_dribble(self):
        self.assertIn("if (CandidateBall->KickByPlayer(this, ChallengeDirection,", self.tackle)
        self.assertIn("TouchCooldownRemaining = FMath::Max(TouchCooldownRemaining, 0.35f)", self.tackle)
        self.assertLess(self.tackle.index("if (CandidateBall->KickByPlayer"),
                        self.tackle.index("TouchCooldownRemaining = FMath::Max(TouchCooldownRemaining, 0.35f)"))

    def test_kicks_once_with_attributed_contact(self):
        self.assertIn("CandidateBall->KickByPlayer(this, ChallengeDirection,", self.tackle)
        self.assertIn("PlayerAttributes->Tackling", self.tackle)


if __name__ == "__main__":
    unittest.main()
