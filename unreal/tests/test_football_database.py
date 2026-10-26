"""Contracts for native football database assets (not an Unreal compile)."""
import pathlib
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
HEADER = (ROOT / "Source/SportsArcade/Public/SportsArcadeFootballDatabase.h").read_text()
SOURCE = (ROOT / "Source/SportsArcade/Private/SportsArcadeFootballDatabase.cpp").read_text()
ROSTER = (ROOT / "Source/SportsArcade/Private/SportsArcadeTeamRoster.cpp").read_text()


class FootballDatabaseContracts(unittest.TestCase):
    def test_blueprint_asset_supports_all_catalog_categories(self):
        self.assertIn("public UDataAsset", HEADER)
        for kind in ("NationRecord", "ClubRecord", "FootballerRecord", "CompetitionRecord"):
            self.assertIn("FSportsArcade" + kind, HEADER)
        for field in ("Nations;", "Clubs;", "Footballers;", "Competitions;"):
            self.assertIn(field, HEADER)

    def test_separate_mens_and_womens_football(self):
        self.assertIn("ESportsArcadeSquadCategory", HEADER)
        self.assertIn("Men,", HEADER)
        self.assertIn("Women", HEADER)
        self.assertIn("Competition.Category", SOURCE)
        self.assertIn("ClubCategories[ClubId] != Competition.Category", SOURCE)

    def test_validates_references_and_unique_ids(self):
        for term in ("NationIds.Contains", "ClubCategories.Contains", "FootballerIds.Contains",
                     "CompetitionIds.Contains", "Participants.Contains", "NationIds.Add",
                     "ClubCategories.Add", "FootballerIds.Add", "CompetitionIds.Add"):
            self.assertIn(term, SOURCE)
        self.assertIn("return Errors.IsEmpty();", SOURCE)

    def test_individual_ratings_flow_from_database_to_match_footballers(self):
        attributes = ("Acceleration", "Agility", "SprintSpeed", "Stamina",
                      "ShortPassing", "Finishing", "BallControl", "Tackling")
        self.assertIn("FindComponentByClass<USportsArcadePlayerAttributes>()", ROSTER)
        for name in attributes:
            self.assertIn("int32 " + name + " = 65;", HEADER)
            self.assertIn("Ratings->" + name + " = FMath::Clamp(Record." + name + ", 1, 99);", ROSTER)

    def test_stadium_defaults_and_club_customisation(self):
        self.assertIn("struct FSportsArcadeStadiumRecord", HEADER)
        self.assertIn("int32 Capacity = 30000;", HEADER)
        self.assertIn("float PitchLengthMetres = 105.f;", HEADER)
        self.assertIn("float PitchWidthMetres = 68.f;", HEADER)
        self.assertIn("FName HomeStadiumId;", HEADER)
        self.assertIn("FName PreferredFormationId;", HEADER)
        self.assertIn("StadiumIds.Contains(Stadium.Id)", SOURCE)
        self.assertIn("Club.HomeStadiumId.IsNone()", SOURCE)
        self.assertIn("Club.PreferredFormationId.IsNone()", SOURCE)
        self.assertIn('FName(TEXT("433"))', ROSTER)
        self.assertIn('FName(TEXT("442"))', ROSTER)

    def test_supports_multiple_competition_formats(self):
        for fmt in ("League", "Knockout", "GroupsAndKnockout"):
            self.assertIn(fmt, HEADER)


if __name__ == "__main__":
    unittest.main()
