#include "SportsArcadeSeasonSave.h"

bool USportsArcadeSeasonSave::StartSeason(FName InCompetitionId, const TArray<FName>& InClubIds,
    bool bHomeAndAway)
{
    if (InCompetitionId.IsNone()) return false;
    TArray<FSportsArcadeLeagueFixture> NewFixtures;
    if (!USportsArcadeFixtures::GenerateLeagueFixtures(InClubIds, bHomeAndAway, NewFixtures))
        return false;

    CompetitionId = InCompetitionId;
    ClubIds = InClubIds;
    Fixtures = MoveTemp(NewFixtures);
    CompletedFixtureIndices.Reset();
    Results.Reset();
    return true;
}

bool USportsArcadeSeasonSave::RecordFixtureResult(int32 FixtureIndex, int32 HomeGoals, int32 AwayGoals)
{
    if (!Fixtures.IsValidIndex(FixtureIndex) || HomeGoals < 0 || AwayGoals < 0 ||
        CompletedFixtureIndices.Contains(FixtureIndex))
        return false;

    const FSportsArcadeLeagueFixture& Fixture = Fixtures[FixtureIndex];
    FSportsArcadeLeagueResult Result;
    Result.HomeClubId = Fixture.HomeClubId;
    Result.AwayClubId = Fixture.AwayClubId;
    Result.HomeGoals = HomeGoals;
    Result.AwayGoals = AwayGoals;

    // Check the table before committing; an invalid save cannot corrupt the season.
    TArray<FSportsArcadeLeagueResult> Candidate = Results;
    Candidate.Add(Result);
    TArray<FSportsArcadeLeagueStanding> CandidateStandings;
    if (!USportsArcadeLeagueTable::CalculateTable(ClubIds, Candidate, CandidateStandings))
        return false;
    Results.Add(Result);
    CompletedFixtureIndices.Add(FixtureIndex);
    return true;
}

bool USportsArcadeSeasonSave::GetCurrentStandings(
    TArray<FSportsArcadeLeagueStanding>& OutStandings) const
{
    return USportsArcadeLeagueTable::CalculateTable(ClubIds, Results, OutStandings);
}
