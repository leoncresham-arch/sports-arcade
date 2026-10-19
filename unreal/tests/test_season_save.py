"""Source contracts for offline Unreal SaveGame league progression."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
HEADER = (ROOT / "Source/SportsArcade/Public/SportsArcadeSeasonSave.h").read_text()
SOURCE = (ROOT / "Source/SportsArcade/Private/SportsArcadeSeasonSave.cpp").read_text()


class SeasonSaveTests(unittest.TestCase):
    def test_save_game_owns_database_ids_and_results(self):
        self.assertIn("public USaveGame", HEADER)
        for field in ("CompetitionId;", "ClubIds;", "Fixtures;", "CompletedFixtureIndices;", "Results;"):
            self.assertIn(field, HEADER)

    def test_season_creation_generates_scheduled_fixtures(self):
        self.assertIn("GenerateLeagueFixtures(InClubIds, bHomeAndAway, NewFixtures)", SOURCE)
        self.assertIn("InCompetitionId.IsNone()", SOURCE)
        self.assertIn("CompletedFixtureIndices.Reset();", SOURCE)
        self.assertIn("Results.Reset();", SOURCE)

    def test_results_reject_invalid_indices_and_duplicates(self):
        self.assertIn("Fixtures.IsValidIndex(FixtureIndex)", SOURCE)
        self.assertIn("CompletedFixtureIndices.Contains(FixtureIndex)", SOURCE)
        self.assertIn("HomeGoals < 0 || AwayGoals < 0", SOURCE)
        self.assertIn("CompletedFixtureIndices.Add(FixtureIndex)", SOURCE)
        self.assertIn("CalculateTable(ClubIds, Candidate, CandidateStandings)", SOURCE)

    def test_native_slot_saving_and_load_validation(self):
        self.assertIn("SaveSeasonToSlot", HEADER)
        self.assertIn("LoadSeasonFromSlot", HEADER)
        self.assertIn("UGameplayStatics::SaveGameToSlot", SOURCE)
        self.assertIn("UGameplayStatics::LoadGameFromSlot", SOURCE)
        self.assertIn("SeenIndices.Contains(FixtureIndex)", SOURCE)
        self.assertIn("Result.HomeClubId != Fixture.HomeClubId", SOURCE)
        self.assertIn("Season->GetCurrentStandings(ValidatedTable)", SOURCE)

    def test_next_match_selection_and_season_end(self):
        self.assertIn("GetNextUnplayedFixtureIndex() const", HEADER)
        self.assertIn("IsSeasonComplete() const", HEADER)
        self.assertIn("!CompletedFixtureIndices.Contains(Index)", SOURCE)
        self.assertIn("return INDEX_NONE;", SOURCE)
        self.assertIn("GetNextUnplayedFixtureIndex() == INDEX_NONE", SOURCE)

    def test_standings_derive_from_saved_results(self):
        self.assertIn("CalculateTable(ClubIds, Results, OutStandings)", SOURCE)


if __name__ == "__main__":
    unittest.main()
