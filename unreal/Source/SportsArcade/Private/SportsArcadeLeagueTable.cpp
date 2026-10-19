#include "SportsArcadeLeagueTable.h"

bool USportsArcadeLeagueTable::CalculateTable(const TArray<FName>& ClubIds,
    const TArray<FSportsArcadeLeagueResult>& Results,
    TArray<FSportsArcadeLeagueStanding>& Standings)
{
    Standings.Reset();
    TMap<FName, int32> Indices;
    for (const FName Club : ClubIds)
    {
        if (Club.IsNone() || Indices.Contains(Club)) return false;
        FSportsArcadeLeagueStanding Entry;
        Entry.ClubId = Club;
        Indices.Add(Club, Standings.Add(Entry));
    }
    if (Standings.Num() < 2)
    {
        Standings.Reset();
        return false;
    }
    for (const FSportsArcadeLeagueResult& Result : Results)
    {
        const int32* HomeIndex = Indices.Find(Result.HomeClubId);
        const int32* AwayIndex = Indices.Find(Result.AwayClubId);
        if (!HomeIndex || !AwayIndex || *HomeIndex == *AwayIndex ||
            Result.HomeGoals < 0 || Result.AwayGoals < 0)
        {
            Standings.Reset();
            return false;
        }
        FSportsArcadeLeagueStanding& Home = Standings[*HomeIndex];
        FSportsArcadeLeagueStanding& Away = Standings[*AwayIndex];
        ++Home.Played;
        ++Away.Played;
        Home.GoalsFor += Result.HomeGoals;
        Home.GoalsAgainst += Result.AwayGoals;
        Away.GoalsFor += Result.AwayGoals;
        Away.GoalsAgainst += Result.HomeGoals;
        if (Result.HomeGoals > Result.AwayGoals)
        {
            ++Home.Won;
            ++Away.Lost;
            Home.Points += 3;
        }
        else if (Result.HomeGoals < Result.AwayGoals)
        {
            ++Away.Won;
            ++Home.Lost;
            Away.Points += 3;
        }
        else
        {
            ++Home.Drawn;
            ++Away.Drawn;
            Home.Points += 1;
            Away.Points += 1;
        }
    }
    Standings.Sort([](const FSportsArcadeLeagueStanding& A, const FSportsArcadeLeagueStanding& B)
    {
        if (A.Points != B.Points) return A.Points > B.Points;
        const int32 DifferenceA = A.GoalsFor - A.GoalsAgainst;
        const int32 DifferenceB = B.GoalsFor - B.GoalsAgainst;
        if (DifferenceA != DifferenceB) return DifferenceA > DifferenceB;
        if (A.GoalsFor != B.GoalsFor) return A.GoalsFor > B.GoalsFor;
        return A.ClubId.ToString() < B.ClubId.ToString();
    });
    return true;
}
