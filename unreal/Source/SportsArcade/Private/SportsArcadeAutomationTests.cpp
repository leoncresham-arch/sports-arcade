#include "SportsArcadeFixtures.h"
#include "SportsArcadeCupFixtures.h"
#include "SportsArcadeLeagueTable.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSportsArcadeLeagueScheduleAutomation,
    "SportsArcade.Seasons.LeagueSchedule",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSportsArcadeLeagueScheduleAutomation::RunTest(const FString& Parameters)
{
    const TArray<FName> Clubs = {FName(TEXT("club_a")), FName(TEXT("club_b")),
        FName(TEXT("club_c")), FName(TEXT("club_d"))};
    TArray<FSportsArcadeLeagueFixture> Fixtures;
    TestTrue(TEXT("Generate complete double-round season"),
        USportsArcadeFixtures::GenerateLeagueFixtures(Clubs, true, Fixtures));
    TestEqual(TEXT("Four clubs play twelve ordered fixtures"), Fixtures.Num(), 12);
    for (int32 Index = 0; Index < Fixtures.Num(); ++Index)
    {
        TestTrue(TEXT("No self-match"), Fixtures[Index].HomeClubId != Fixtures[Index].AwayClubId);
        TestTrue(TEXT("First half contains only opening-leg rounds"),
            Index >= 6 || Fixtures[Index].Round <= 3);
        TestTrue(TEXT("Second half contains only return-leg rounds"),
            Index < 6 || Fixtures[Index].Round >= 4);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSportsArcadeCupBracketAutomation,
    "SportsArcade.Seasons.CupBracket",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSportsArcadeCupBracketAutomation::RunTest(const FString& Parameters)
{
    const TArray<FName> Clubs = {FName(TEXT("club_a")), FName(TEXT("club_b")),
        FName(TEXT("club_c")), FName(TEXT("club_d"))};
    TArray<FSportsArcadeCupFixture> Opening;
    TestTrue(TEXT("Opening cup bracket accepts four teams"),
        USportsArcadeCupFixtures::GenerateOpeningRound(Clubs, Opening));
    TestEqual(TEXT("Two opening ties"), Opening.Num(), 2);
    TArray<FName> Winners = {FName(TEXT("club_a")), FName(TEXT("club_d"))};
    TArray<FSportsArcadeCupFixture> Final;
    TestTrue(TEXT("Winners reach final"),
        USportsArcadeCupFixtures::GenerateNextRound(Opening, Winners, Final));
    TestEqual(TEXT("One final match"), Final.Num(), 1);
    if (Final.Num() == 1)
    {
        TestEqual(TEXT("Final is round two"), Final[0].Round, 2);
        TestEqual(TEXT("First finalist"), Final[0].HomeClubId, Winners[0]);
        TestEqual(TEXT("Second finalist"), Final[0].AwayClubId, Winners[1]);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSportsArcadeTableAutomation,
    "SportsArcade.Seasons.LeagueTable",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSportsArcadeTableAutomation::RunTest(const FString& Parameters)
{
    const TArray<FName> Clubs = {FName(TEXT("club_a")), FName(TEXT("club_b"))};
    FSportsArcadeLeagueResult Result;
    Result.HomeClubId = Clubs[0];
    Result.AwayClubId = Clubs[1];
    Result.HomeGoals = 2;
    Result.AwayGoals = 1;
    TArray<FSportsArcadeLeagueStanding> Table;
    TArray<FSportsArcadeLeagueResult> Results;
    Results.Add(Result);
    TestTrue(TEXT("Calculate a match table"),
        USportsArcadeLeagueTable::CalculateTable(Clubs, Results, Table));
    TestEqual(TEXT("Both clubs have standings"), Table.Num(), 2);
    if (Table.Num() == 2)
    {
        TestEqual(TEXT("Winner gets three points"), Table[0].Points, 3);
        TestEqual(TEXT("Loser gets no points"), Table[1].Points, 0);
    }
    return true;
}

#endif
