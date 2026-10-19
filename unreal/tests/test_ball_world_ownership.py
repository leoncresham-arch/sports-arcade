"""Source checks for world-safe physics contacts. Not an Unreal Editor simulation."""
from pathlib import Path
import unittest

SOURCE = (Path(__file__).resolve().parents[1] / "Source/SportsArcade/Private/SportsArcadeBall.cpp").read_text()


class BallOwnershipContract(unittest.TestCase):
    def test_attribution_rejects_foreign_world(self):
        body = SOURCE.split("int32 ASportsArcadeBall::GetLastTouchTeamIndex() const", 1)[1].split("\n}", 1)[0]
        self.assertIn("Player->GetWorld() == GetWorld()", body)

    def test_physics_contacts_reject_nonfinite_values(self):
        for fragment in (
            "Direction.ContainsNaN()", "DesiredVelocity.ContainsNaN()",
            "!FMath::IsFinite(StrengthKgCmPerSec)",
            "!FMath::IsFinite(MaxContactDistanceCm)",
            "!FMath::IsFinite(MaxDistanceCm)",
        ):
            self.assertIn(fragment, SOURCE)

    def test_player_contacts_reject_cross_world_actors(self):
        for signature in (
            "bool ASportsArcadeBall::KickByPlayer(",
            "bool ASportsArcadeBall::ApplyControlledTouch(",
        ):
            body = SOURCE.split(signature, 1)[1].split("\n}", 1)[0]
            self.assertIn("Player->GetWorld() != GetWorld()", body)
            self.assertLess(body.index("Player->GetWorld() != GetWorld()"),
                            body.index("LastTouchPlayer = Player;"))


if __name__ == "__main__":
    unittest.main()
