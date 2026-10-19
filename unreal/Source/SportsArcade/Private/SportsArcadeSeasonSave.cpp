#include "SportsArcadeSeasonSave.h"
#include "Kismet/GameplayStatics.h"

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

bool USportsArcadeSeasonSave::SaveSeasonToSlot(const FString& SlotName, int32 UserIndex) const
{
    if (SlotName.IsEmpty() || UserIndex < 0 || CompetitionId.IsNone() || Fixtures.IsEmpty() ||
        CompletedFixtureIndices.Num() != Results.Num())
        return false;
    TArray<FSportsArcadeLeagueStanding> ValidatedTable;
    if (!GetCurrentStandings(ValidatedTable)) return false;
    return UGameplayStatics::SaveGameToSlot(const_cast<USportsArcadeSeasonSave*>(this), SlotName, UserIndex);
}

USportsArcadeSeasonSave* USportsArcadeSeasonSave::LoadSeasonFromSlot(const UObject* WorldContextObject,
    const FString& SlotName, int32 UserIndex)
{
    if (!IsValid(WorldContextObject) || SlotName.IsEmpty() || UserIndex < 0) return nullptr;
    USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);
    USportsArcadeSeasonSave* Season = Cast<USportsArcadeSeasonSave>(Loaded);
    if (!IsValid(Season) || Season->CompetitionId.IsNone() ||
        Season->Fixtures.IsEmpty() ||
        Season->CompletedFixtureIndices.Num() != Season->Results.Num())
        return nullptr;
    TSet<int32> SeenIndices;
    for (int32 Index = 0; Index < Season->CompletedFixtureIndices.Num(); ++Index)
    {
        const int32 FixtureIndex = Season->CompletedFixtureIndices[Index];
        if (!Season->Fixtures.IsValidIndex(FixtureIndex) || SeenIndices.Contains(FixtureIndex))
            return nullptr;
        SeenIndices.Add(FixtureIndex);
        const FSportsArcadeLeagueFixture& Fixture = Season->Fixtures[FixtureIndex];
        const FSportsArcadeLeagueResult& Result = Season->Results[Index];
        if (Result.HomeClubId != Fixture.HomeClubId || Result.AwayClubId != Fixture.AwayClubId)
            return nullptr;
    }
    TArray<FSportsArcadeLeagueStanding> ValidatedTable;
    return Season->GetCurrentStandings(ValidatedTable) ? Season : nullptr;
}
