from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
HEADER = (ROOT / "Source/SportsArcade/Public/SportsArcadeSeasonFactory.h").read_text()
SOURCE = (ROOT / "Source/SportsArcade/Private/SportsArcadeSeasonFactory.cpp").read_text()

class SeasonFactoryTests(unittest.TestCase):
    def test_blueprint_league_creation(self):
        self.assertIn("CreateLeagueSeason(", HEADER)
        self.assertIn("Catalog->ValidateDatabase(Errors)", SOURCE)
        self.assertIn("Competition.Format != ESportsArcadeCompetitionFormat::League", SOURCE)
        self.assertIn("Competition.ParticipantClubIds.Num() < 2", SOURCE)
        self.assertIn("Season->StartSeason(Competition.Id, Competition.ParticipantClubIds, bHomeAndAway)", SOURCE)

if __name__ == "__main__":
    unittest.main()
