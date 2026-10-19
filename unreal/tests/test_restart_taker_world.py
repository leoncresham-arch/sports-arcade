"""Source validation for a single authoritative restart eligibility query."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "Source/SportsArcade/Private/SportsArcadeGameMode.cpp").read_text()
HEADER = (ROOT / "Source/SportsArcade/Public/SportsArcadeGameMode.h").read_text()


class RestartOwnershipTests(unittest.TestCase):
    def test_world_and_team_eligibility(self):
        query = SOURCE.split("bool ASportsArcadeGameMode::CanResumePendingRestart() const", 1)[1].split("\n}", 1)[0]
        self.assertIn("PendingRestartTaker->GetWorld() == GetWorld()", query)
        self.assertIn("PendingRestartTaker->TeamIndex == PendingRestartTeam", query)
        self.assertIn("PendingRestartType != ESportsArcadeRestartType::Undetermined", query)
        self.assertIn("bool CanResumePendingRestart() const;", HEADER)

    def test_roster_taker_selection_rejects_foreign_world(self):
        method = SOURCE.split("ASportsArcadeCharacter* ASportsArcadeGameMode::FindRestartTaker(", 1)[1].split("\n}", 1)[0]
        self.assertIn("Candidate->GetWorld() != GetWorld()", method)
        self.assertIn("Candidate->TeamIndex == TakingTeam && Candidate->GetWorld() == GetWorld()", SOURCE)
        self.assertIn("!IsValid(*It) || It->TeamIndex != Team || It->GetWorld() != GetWorld()", method)

    def test_referee_stoppage_only_clears_after_restart(self):
        method = SOURCE.split("void ASportsArcadeGameMode::ResumeAfterBallOut()", 1)[1].split("\n}", 1)[0]
        self.assertIn("if (!CanResumePendingRestart()) return;", method)
        self.assertLess(method.index("if (!CanResumePendingRestart()) return;"),
                        method.index("SetActorLocation(TakerPosition"))
        self.assertLess(method.index("if (!MatchBall->KickByPlayer("),
                        method.index("bBallOutOfPlay = false;"))


if __name__ == "__main__":
    unittest.main()
