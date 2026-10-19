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
    def test_missing_cup_mapping_clears_stale_player_ids(self):
        self.assertIn("Home->ClearFootballerRecordIds()", cpp)
        self.assertIn("Away->ClearFootballerRecordIds()", cpp)

    def test_cup_teams_use_validated_catalog(self):
        self.assertIn("HomeClub.Category != AwayClub.Category", cpp)
        self.assertIn("Home->CanAssignFootballerRecords(FootballCatalog)", cpp)
        self.assertIn("Away->CanAssignFootballerRecords(FootballCatalog)", cpp)

    def test_completed_tie_cannot_be_replayed(self):
        self.assertIn("BoundCup->CurrentRoundWinners.Num() > BoundCupFixtureIndex", cpp)
        self.assertIn("BoundCupFixtureIndex = Cup->CurrentRoundWinners.Num()", cpp)
        self.assertIn("BoundCupFixtureIndex != BoundCup->CurrentRoundWinners.Num()", cpp)

    def test_draw_does_not_pick_winner(self):
        self.assertIn("HomeGoals == AwayGoals", cpp)
        self.assertIn("BoundCup->RecordWinner(", cpp)
        self.assertIn("CommitCupResult();", cpp)
    def test_completed_round_progression(self):
        self.assertIn("AdvanceBoundCupRound()", header)
        self.assertIn("BoundCup->AdvanceRound()", cpp)
        self.assertIn("return BindNextCupFixture(BoundCup);", cpp)

    def test_cup_ui_can_access_progress_and_save(self):
        self.assertIn("GetBoundCup() const", header)
        self.assertIn("SaveBoundCupToSlot", header)
        self.assertIn("BoundCup->SaveCupToSlot(SlotName, UserIndex)", cpp)

    def test_penalty_winner_resolves_draw(self):
        self.assertIn("StartCupPenaltyShootout", header)
        self.assertIn("RecordCupPenaltyAttempt", header)
        self.assertIn("GetCupPenaltyShootout", header)
        self.assertIn("ActivePenaltyShootout->RecordAttempt(KickingTeam, bScored)", cpp)
        self.assertIn("bCupResultRecorded = BoundCup->RecordWinner(", cpp)

    def test_unresolved_draw_is_exposed_for_tie_break_ui(self):
        self.assertIn("IsCupTieBreakRequired() const", header)
        self.assertIn("!bCupResultRecorded && HomeGoals == AwayGoals", cpp)

    def test_post_match_result(self):
        self.assertIn("IsCupResultRecorded() const", header)
        self.assertIn("bMatchFinished && bCupResultRecorded", cpp)
