"""Static checks for deterministic league table, pending native UE compilation."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
HEADER = (ROOT / "Source/SportsArcade/Public/SportsArcadeLeagueTable.h").read_text()
SOURCE = (ROOT / "Source/SportsArcade/Private/SportsArcadeLeagueTable.cpp").read_text()


class LeagueTableContracts(unittest.TestCase):
    def test_exposes_results_and_standings_to_blueprints(self):
        for name in ("FSportsArcadeLeagueResult", "FSportsArcadeLeagueStanding", "CalculateTable"):
            self.assertIn(name, HEADER)
        for metric in ("Played", "Won", "Drawn", "Lost", "GoalsFor", "GoalsAgainst", "Points"):
            self.assertIn(metric, HEADER)

    def test_scoring_and_tie_breaks(self):
        self.assertIn("Home.Points += 3", SOURCE)
        self.assertIn("Away.Points += 3", SOURCE)
        self.assertIn("Home.Points += 1", SOURCE)
        self.assertIn("Away.Points += 1", SOURCE)
        self.assertIn("A.Points != B.Points", SOURCE)
        self.assertIn("A.GoalsFor - A.GoalsAgainst", SOURCE)
        self.assertIn("A.ClubId.ToString() < B.ClubId.ToString()", SOURCE)

    def test_invalid_matches_do_not_generate_partial_tables(self):
        self.assertIn("Result.HomeGoals < 0 || Result.AwayGoals < 0", SOURCE)
        self.assertIn("*HomeIndex == *AwayIndex", SOURCE)
        self.assertIn("Standings.Reset();", SOURCE)
        self.assertIn("return false;", SOURCE)


if __name__ == "__main__":
    unittest.main()
