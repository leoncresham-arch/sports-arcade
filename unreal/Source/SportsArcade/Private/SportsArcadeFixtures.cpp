#include "SportsArcadeFixtures.h"

bool USportsArcadeFixtures::GenerateLeagueFixtures(const TArray<FName>& ClubIds, bool bHomeAndAway,
    TArray<FSportsArcadeLeagueFixture>& Fixtures)
{
    Fixtures.Reset();
    if (ClubIds.Num() < 2) return false;
    TSet<FName> Seen;
    for (const FName Id : ClubIds)
    {
        if (Id.IsNone() || Seen.Contains(Id)) return false;
        Seen.Add(Id);
    }

    TArray<FName> Rotation = ClubIds;
    if (Rotation.Num() % 2 != 0) Rotation.Add(NAME_None); // Bye marker.
    const int32 Count = Rotation.Num();
    const int32 Rounds = Count - 1;
    const int32 MatchesPerRound = Count / 2;

    for (int32 RoundIndex = 0; RoundIndex < Rounds; ++RoundIndex)
    {
        for (int32 MatchIndex = 0; MatchIndex < MatchesPerRound; ++MatchIndex)
        {
            const FName Left = Rotation[MatchIndex];
            const FName Right = Rotation[Count - 1 - MatchIndex];
            if (Left.IsNone() || Right.IsNone()) continue;

            FSportsArcadeLeagueFixture Fixture;
            Fixture.Round = RoundIndex + 1;
            // Alternate orientation while rotating for a balanced first leg.
            const bool bSwap = ((RoundIndex + MatchIndex) % 2) != 0;
            Fixture.HomeClubId = bSwap ? Right : Left;
            Fixture.AwayClubId = bSwap ? Left : Right;
            Fixtures.Add(Fixture);
            if (bHomeAndAway)
            {
                FSportsArcadeLeagueFixture ReturnFixture;
                ReturnFixture.Round = RoundIndex + 1 + Rounds;
                ReturnFixture.HomeClubId = Fixture.AwayClubId;
                ReturnFixture.AwayClubId = Fixture.HomeClubId;
                Fixtures.Add(ReturnFixture);
            }
        }

        // Fixed first entry: classic circle method for all even-sized rotations.
        const FName Last = Rotation.Pop(EAllowShrinking::No);
        Rotation.Insert(Last, 1);
    }
    return true;
}
