"""Native league-to-match integration contracts, pending UE runtime tests."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
HEADER = (ROOT / "Source/SportsArcade/Public/SportsArcadeGameMode.h").read_text()
SOURCE = (ROOT / "Source/SportsArcade/Private/SportsArcadeGameMode.cpp").read_text()


class SeasonMatchIntegrationTests(unittest.TestCase):
    def test_blueprint_fixture_selection_exposes_both_clubs(self):
        self.assertIn("bool BindSeasonFixture(USportsArcadeSeasonSave* Season, int32 FixtureIndex)", HEADER)
        self.assertIn("SelectedHomeClubId", HEADER)
        self.assertIn("SelectedAwayClubId", HEADER)
        self.assertIn("SelectedHomeClubId = Fixture.HomeClubId;", SOURCE)
        self.assertIn("SelectedAwayClubId = Fixture.AwayClubId;", SOURCE)

    def test_rejects_invalid_or_finished_fixtures(self):
        self.assertIn("Season->Fixtures.IsValidIndex(FixtureIndex)", SOURCE)
        self.assertIn("Season->CompletedFixtureIndices.Contains(FixtureIndex)", SOURCE)
        self.assertIn("Fixture.HomeClubId == Fixture.AwayClubId", SOURCE)
        self.assertIn("ElapsedMatchSeconds > 0.f && !bMatchFinished", SOURCE)

    def test_explicit_slot_save_from_match_mode(self):
        self.assertIn("SaveBoundSeasonToSlot", HEADER)
        self.assertIn("IsBoundSeasonResultRecorded", HEADER)
        self.assertIn("BoundSeason->SaveSeasonToSlot(SlotName, UserIndex)", SOURCE)
        self.assertIn("bMatchFinished && bSeasonResultRecorded", SOURCE)

    def test_next_fixture_requires_finished_recorded_match(self):
        self.assertIn("AdvanceToNextSeasonFixture", HEADER)
        self.assertIn("!bMatchFinished || !bSeasonResultRecorded", SOURCE)
        self.assertIn("BoundSeason->GetNextUnplayedFixtureIndex()", SOURCE)
        self.assertIn("BindSeasonFixture(BoundSeason, NextIndex)", SOURCE)

    def test_completed_fixture_cannot_be_reset_or_rebound_mid_match(self):
        self.assertIn("BoundSeason->CompletedFixtureIndices.Contains(BoundFixtureIndex)", SOURCE)
        self.assertIn("!bMatchFinished && BoundFixtureIndex != INDEX_NONE", SOURCE)

    def test_result_only_submitted_at_full_time(self):
        self.assertIn("CommitSeasonResult();\n        OnFullTime.Broadcast();", SOURCE)
        self.assertIn("bSeasonResultRecorded || !IsValid(BoundSeason)", SOURCE)
        self.assertIn("BoundSeason->RecordFixtureResult(\n        BoundFixtureIndex, HomeGoals, AwayGoals)", SOURCE)
        self.assertIn("bSeasonResultRecorded = false;", SOURCE)


if __name__ == "__main__":
    unittest.main()
