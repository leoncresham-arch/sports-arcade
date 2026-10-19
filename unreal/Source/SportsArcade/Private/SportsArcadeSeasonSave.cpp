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
    if (!ValidateProgress()) return false;
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
    return Season->ValidateProgress() ? Season : nullptr;
}

int32 USportsArcadeSeasonSave::GetNextUnplayedFixtureIndex() const
{
    for (int32 Index = 0; Index < Fixtures.Num(); ++Index)
    {
        if (!CompletedFixtureIndices.Contains(Index))
            return Index;
    }
    return INDEX_NONE;
}

bool USportsArcadeSeasonSave::IsSeasonComplete() const
{
    return !Fixtures.IsEmpty() && GetNextUnplayedFixtureIndex() == INDEX_NONE;
}

bool USportsArcadeSeasonSave::ValidateProgress() const
{
    if (CompetitionId.IsNone() || Fixtures.IsEmpty() ||
        CompletedFixtureIndices.Num() != Results.Num()) return false;
    TSet<int32> SeenIndices;
    for (int32 Index = 0; Index < CompletedFixtureIndices.Num(); ++Index)
    {
        const int32 FixtureIndex = CompletedFixtureIndices[Index];
        if (!Fixtures.IsValidIndex(FixtureIndex) || SeenIndices.Contains(FixtureIndex))
            return false;
        SeenIndices.Add(FixtureIndex);
        const FSportsArcadeLeagueFixture& Fixture = Fixtures[FixtureIndex];
        const FSportsArcadeLeagueResult& Result = Results[Index];
        if (Result.HomeClubId != Fixture.HomeClubId || Result.AwayClubId != Fixture.AwayClubId)
            return false;
    }
    TArray<FSportsArcadeLeagueStanding> Standings;
    return GetCurrentStandings(Standings);
}

int32 USportsArcadeSeasonSave::GetCompletedFixtureCount() const
{
    return CompletedFixtureIndices.Num();
}

float USportsArcadeSeasonSave::GetSeasonProgress() const
{
    return Fixtures.IsEmpty() ? 0.f : FMath::Clamp(
        static_cast<float>(CompletedFixtureIndices.Num()) / static_cast<float>(Fixtures.Num()), 0.f, 1.f);
}

bool USportsArcadeSeasonSave::GetNextUnplayedFixture(FSportsArcadeLeagueFixture& OutFixture) const
{
    const int32 Index = GetNextUnplayedFixtureIndex();
    if (!Fixtures.IsValidIndex(Index)) return false;
    OutFixture = Fixtures[Index];
    return true;
}
