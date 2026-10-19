"""Source contracts for season roster bindings."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
game = (ROOT / "Source/SportsArcade/Private/SportsArcadeGameMode.cpp").read_text()
roster = (ROOT / "Source/SportsArcade/Public/SportsArcadeTeamRoster.h").read_text()

class SeasonRosters(unittest.TestCase):
    def test_club_ids_are_exposed(self):
        self.assertIn("FName ClubId;", roster)
        self.assertIn("HomeRoster->ClubId = Fixture.HomeClubId", game)
        self.assertIn("AwayRoster->ClubId = Fixture.AwayClubId", game)

    def test_complete_rosters_are_required(self):
        self.assertIn("HomeRoster->HasCompleteLineup()", game)
        self.assertIn("AwayRoster->HasCompleteLineup()", game)
        self.assertLess(game.index("!HomeRoster->HasCompleteLineup()"),
                        game.index("HomeRoster->ClubId = Fixture.HomeClubId"))
