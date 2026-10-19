from pathlib import Path
import unittest
ROOT = Path(__file__).resolve().parents[1]
header = (ROOT / "Source/SportsArcade/Public/SportsArcadeGameMode.h").read_text()
cpp = (ROOT / "Source/SportsArcade/Private/SportsArcadeGameMode.cpp").read_text()
class CupMatchIntegration(unittest.TestCase):
    def test_knockout_binding(self):
        self.assertIn("BindNextCupFixture(USportsArcadeCupSave* Cup)", header)
        self.assertIn("Cup->CurrentRoundFixtures[Cup->CurrentRoundWinners.Num()]", cpp)
        self.assertIn("BoundCup = Cup;", cpp)
    def test_draw_does_not_pick_winner(self):
        self.assertIn("HomeGoals == AwayGoals", cpp)
        self.assertIn("BoundCup->RecordWinner(", cpp)
        self.assertIn("CommitCupResult();", cpp)
    def test_completed_round_progression(self):
        self.assertIn("AdvanceBoundCupRound()", header)
        self.assertIn("BoundCup->AdvanceRound()", cpp)
        self.assertIn("return BindNextCupFixture(BoundCup);", cpp)

    def test_post_match_result(self):
        self.assertIn("IsCupResultRecorded() const", header)
        self.assertIn("bMatchFinished && bCupResultRecorded", cpp)
