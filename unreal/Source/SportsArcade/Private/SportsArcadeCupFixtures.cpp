#include "SportsArcadeCupFixtures.h"

bool USportsArcadeCupFixtures::GenerateOpeningRound(
    const TArray<FName>& ClubIds, TArray<FSportsArcadeCupFixture>& OutFixtures)
{
    OutFixtures.Reset();
    const int32 Count = ClubIds.Num();
    if (Count < 2 || Count > 64 || (Count & (Count - 1)) != 0) return false;
    TSet<FName> Seen;
    for (FName Club : ClubIds)
    {
        if (Club.IsNone() || Seen.Contains(Club)) return false;
        Seen.Add(Club);
    }
    for (int32 Index = 0; Index < Count; Index += 2)
    {
        FSportsArcadeCupFixture Fixture;
        Fixture.Round = 1;
        Fixture.HomeClubId = ClubIds[Index];
        Fixture.AwayClubId = ClubIds[Index + 1];
        OutFixtures.Add(Fixture);
    }
    return true;
}

bool USportsArcadeCupFixtures::GenerateNextRound(
    const TArray<FSportsArcadeCupFixture>& PreviousRound,
    const TArray<FName>& Winners, TArray<FSportsArcadeCupFixture>& OutFixtures)
{
    OutFixtures.Reset();
    if (PreviousRound.Num() < 2 || PreviousRound.Num() != Winners.Num() ||
        (PreviousRound.Num() & (PreviousRound.Num() - 1)) != 0)
        return false;

    TSet<FName> Seen;
    const int32 PreviousRoundNumber = PreviousRound[0].Round;
    for (int32 Index = 0; Index < PreviousRound.Num(); ++Index)
    {
        const FSportsArcadeCupFixture& Played = PreviousRound[Index];
        const FName Winner = Winners[Index];
        if (PreviousRoundNumber < 1 || Played.Round != PreviousRoundNumber ||
            Winner.IsNone() || Seen.Contains(Winner) ||
            (Winner != Played.HomeClubId && Winner != Played.AwayClubId))
            return false;
        Seen.Add(Winner);
    }
    for (int32 Index = 0; Index < Winners.Num(); Index += 2)
    {
        FSportsArcadeCupFixture Fixture;
        Fixture.Round = PreviousRoundNumber + 1;
        Fixture.HomeClubId = Winners[Index];
        Fixture.AwayClubId = Winners[Index + 1];
        OutFixtures.Add(Fixture);
    }
    return true;
}
