"""Contracts for optional season club catalog validation."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
header = (ROOT / "Source/SportsArcade/Public/SportsArcadeGameMode.h").read_text()
game = (ROOT / "Source/SportsArcade/Private/SportsArcadeGameMode.cpp").read_text()
catalog_h = (ROOT / "Source/SportsArcade/Public/SportsArcadeFootballDatabase.h").read_text()
catalog_cpp = (ROOT / "Source/SportsArcade/Private/SportsArcadeFootballDatabase.cpp").read_text()

class SeasonCatalogContracts(unittest.TestCase):
    def test_native_lookup(self):
        self.assertIn("FindClubById(FName ClubId", catalog_h)
        self.assertIn("Club.Id == ClubId", catalog_cpp)

    def test_catalog_player_mapping_on_fixture_selection(self):
        self.assertIn("HomeRoster->AssignFootballerRecords(FootballCatalog)", game)
        self.assertIn("AwayRoster->AssignFootballerRecords(FootballCatalog)", game)

    def test_season_match_catalog_check(self):
        self.assertIn("TObjectPtr<USportsArcadeFootballDatabase> FootballCatalog;", header)
        self.assertIn("FootballCatalog->FindClubById(Fixture.HomeClubId, HomeClub)", game)
        self.assertIn("FootballCatalog->FindClubById(Fixture.AwayClubId, AwayClub)", game)
        self.assertIn("HomeClub.Category != AwayClub.Category", game)

if __name__ == "__main__":
    unittest.main()
