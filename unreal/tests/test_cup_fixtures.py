from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
HEADER = (ROOT / "Source/SportsArcade/Public/SportsArcadeCupFixtures.h").read_text()
CPP = (ROOT / "Source/SportsArcade/Private/SportsArcadeCupFixtures.cpp").read_text()

class CupFixturesTests(unittest.TestCase):
    def test_exposes_round_generation_to_blueprints(self):
        self.assertIn("GenerateOpeningRound", HEADER)
        self.assertIn("GenerateNextRound", HEADER)
        self.assertIn("TArray<FSportsArcadeCupFixture>", HEADER)

    def test_power_of_two_and_uniqueness(self):
        self.assertIn("(Count & (Count - 1)) != 0", CPP)
        self.assertIn("Count < 2 || Count > 64", CPP)
        self.assertIn("Seen.Contains(Club)", CPP)

    def test_winner_belongs_to_played_fixture(self):
        self.assertIn("Winner != Played.HomeClubId && Winner != Played.AwayClubId", CPP)
        self.assertIn("Seen.Contains(Winner)", CPP)
        self.assertIn("Fixture.Round = PreviousRoundNumber + 1", CPP)

if __name__ == "__main__":
    unittest.main()
