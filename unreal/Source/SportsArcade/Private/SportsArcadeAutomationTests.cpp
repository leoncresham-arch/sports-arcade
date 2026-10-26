#include "SportsArcadeFixtures.h"
#include "SportsArcadeCupFixtures.h"
#include "SportsArcadeCupSave.h"
#include "SportsArcadeLeagueTable.h"
#include "SportsArcadeFootballDatabase.h"
#include "SportsArcadePlayerAttributes.h"
#include "SportsArcadePenaltyShootout.h"

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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSportsArcadePenaltyShootoutAutomation,
    "SportsArcade.Rules.PenaltyShootout",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSportsArcadePenaltyShootoutAutomation::RunTest(const FString& Parameters)
{
    USportsArcadePenaltyShootout* Shootout =
        NewObject<USportsArcadePenaltyShootout>(GetTransientPackage());
    TestNotNull(TEXT("Shootout state available"), Shootout);
    if (!Shootout) return false;

    TestEqual(TEXT("Home kicks first"), Shootout->GetNextKickingTeam(), 0);
    TestEqual(TEXT("Opening shooter slot"), Shootout->GetNextKickerLineupSlot(), 9);
    TestTrue(TEXT("Home first attempt"), Shootout->RecordAttempt(0, true));
    TestEqual(TEXT("Away first shooter slot"), Shootout->GetNextKickerLineupSlot(), 9);
    TestFalse(TEXT("Cannot kick twice"), Shootout->RecordAttempt(0, false));
    TestTrue(TEXT("Away reply"), Shootout->RecordAttempt(1, false));
    TestEqual(TEXT("Home second shooter slot"), Shootout->GetNextKickerLineupSlot(), 10);
    TestTrue(TEXT("Home second attempt"), Shootout->RecordAttempt(0, true));
    TestTrue(TEXT("Away second miss"), Shootout->RecordAttempt(1, false));
    TestTrue(TEXT("Home third attempt"), Shootout->RecordAttempt(0, true));
    TestTrue(TEXT("Away third miss"), Shootout->RecordAttempt(1, false));
    TestEqual(TEXT("Insurmountable three-goal lead wins shootout"), Shootout->WinningTeam, 0);
    TestFalse(TEXT("No additional kicks once decided"), Shootout->RecordAttempt(0, true));
    Shootout->ResetShootout();
    Shootout->HomeAttempts = 0;
    Shootout->AwayAttempts = 1;
    TestEqual(TEXT("Invalid attempt ordering cannot choose a kicker"),
        Shootout->GetNextKickingTeam(), INDEX_NONE);
    TestFalse(TEXT("Invalid ordering cannot record a kick"),
        Shootout->RecordAttempt(0, true));
    Shootout->ResetShootout();
    Shootout->HomeScored = 1;
    TestEqual(TEXT("Goals cannot exceed attempts"),
        Shootout->GetNextKickingTeam(), INDEX_NONE);
    Shootout->ResetShootout();
    Shootout->HomeAttempts = 11;
    Shootout->AwayAttempts = 11;
    Shootout->HomeScored = 7;
    Shootout->AwayScored = 7;
    TestEqual(TEXT("Twelfth home kick wraps to opening striker"),
        Shootout->GetNextKickerLineupSlot(), 9);
    Shootout->HomeAttempts = 12;
    TestEqual(TEXT("Twelfth away kick also wraps to opening striker"),
        Shootout->GetNextKickerLineupSlot(), 9);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSportsArcadeCupSaveIntegrityAutomation,
    "SportsArcade.Seasons.CupSaveIntegrity",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSportsArcadeCupSaveIntegrityAutomation::RunTest(const FString& Parameters)
{
    USportsArcadeCupSave* Cup = NewObject<USportsArcadeCupSave>(GetTransientPackage());
    TestNotNull(TEXT("Cup progress object"), Cup);
    if (!Cup) return false;
    const TArray<FName> Clubs = {FName(TEXT("a")), FName(TEXT("b")),
        FName(TEXT("c")), FName(TEXT("d"))};
    TestTrue(TEXT("Create four-team cup"), Cup->StartCup(FName(TEXT("test_cup")), Clubs));
    TestTrue(TEXT("Opening state validates"), Cup->ValidateProgress());
    TestTrue(TEXT("First tie result"), Cup->RecordWinner(Clubs[0]));
    TestTrue(TEXT("Second tie result"), Cup->RecordWinner(Clubs[2]));
    TestTrue(TEXT("Completed opening round validates"), Cup->ValidateProgress());
    Cup->CompletedTies = 99;
    TestFalse(TEXT("Impossible completed-tie count is rejected"), Cup->ValidateProgress());
    TestFalse(TEXT("Corrupt cup cannot advance"), Cup->AdvanceRound());
    Cup->CompletedTies = 2;
    Cup->CurrentRoundFixtures[0].Round = 9;
    TestFalse(TEXT("Incorrect round index rejected"), Cup->ValidateProgress());
    Cup->CurrentRoundFixtures[0].Round = 1;
    TestTrue(TEXT("Valid cup advances"), Cup->AdvanceRound());
    TestTrue(TEXT("Final state validates"), Cup->ValidateProgress());
    TestTrue(TEXT("Champion crowned"), Cup->RecordWinner(Clubs[0]));
    TestTrue(TEXT("Completed tournament validates"), Cup->ValidateProgress());
    TestEqual(TEXT("Champion identity"), Cup->ChampionId, Clubs[0]);
    return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSportsArcadeHomeStadiumAutomation,
    "SportsArcade.Database.HomeStadiumFallback",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSportsArcadeHomeStadiumAutomation::RunTest(const FString& Parameters)
{
    USportsArcadeFootballDatabase* Catalog =
        NewObject<USportsArcadeFootballDatabase>(GetTransientPackage());
    TestNotNull(TEXT("Football database exists"), Catalog);
    if (!Catalog) return false;

    const FSportsArcadeStadiumRecord Default =
        Catalog->GetHomeStadiumOrDefault(FName(TEXT("unknown")));
    TestEqual(TEXT("Missing club uses generic stadium"),
        Default.Id, FName(TEXT("GenericStadium")));
    TestEqual(TEXT("Generic pitch length"), Default.PitchLengthMetres, 105.f);
    TestEqual(TEXT("Generic pitch width"), Default.PitchWidthMetres, 68.f);

    FSportsArcadeNationRecord Nation;
    Nation.Id = FName(TEXT("test_nation"));
    Nation.DisplayName = TEXT("Test Nation");
    Catalog->Nations.Add(Nation);

    FSportsArcadeClubRecord Club;
    Club.Id = FName(TEXT("home_club"));
    Club.DisplayName = TEXT("Home Club");
    Club.NationId = Nation.Id;
    Club.HomeStadiumId = FName(TEXT("arena_a"));
    Club.bRequireVerifiedHomeStadium = true;
    Catalog->Clubs.Add(Club);

    TArray<FString> Errors;
    TestFalse(TEXT("Missing required ground invalidates database"),
        Catalog->ValidateDatabase(Errors));

    FSportsArcadeStadiumRecord Stadium;
    Stadium.Id = Club.HomeStadiumId;
    Stadium.DisplayName = TEXT("Original Arena");
    Stadium.Capacity = 42000;
    Stadium.PitchLengthMetres = 104.f;
    Stadium.PitchWidthMetres = 67.f;
    Catalog->Stadiums.Add(Stadium);
    TestTrue(TEXT("Complete club and ground records validate"),
        Catalog->ValidateDatabase(Errors));

    const FSportsArcadeStadiumRecord Resolved =
        Catalog->GetHomeStadiumOrDefault(Club.Id);
    TestEqual(TEXT("Home club gets correct stadium"), Resolved.Id, Stadium.Id);
    TestEqual(TEXT("Authored pitch length"), Resolved.PitchLengthMetres, 104.f);
    TestEqual(TEXT("Authored pitch width"), Resolved.PitchWidthMetres, 67.f);

    Catalog->Stadiums[0].PitchLengthMetres = 200.f;
    TestFalse(TEXT("Out-of-bounds stadium invalidates catalog"),
        Catalog->ValidateDatabase(Errors));
    TestEqual(TEXT("Invalid stadium falls back safely"),
        Catalog->GetHomeStadiumOrDefault(Club.Id).Id,
        FName(TEXT("GenericStadium")));
    return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSportsArcadePlayerRatingAutomation,
    "SportsArcade.Gameplay.PlayerRatingScaling",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSportsArcadePlayerRatingAutomation::RunTest(const FString& Parameters)
{
    // Exercise the rating function used by speed, fatigue and football actions.
    const float Low = USportsArcadePlayerAttributes::RatingFactor(1);
    const float Neutral = USportsArcadePlayerAttributes::RatingFactor(65);
    const float High = USportsArcadePlayerAttributes::RatingFactor(99);
    TestTrue(TEXT("Low rating has a physical disadvantage"), Low < Neutral);
    TestTrue(TEXT("Neutral rating preserves baseline"), FMath::IsNearlyEqual(Neutral, 1.f));
    TestTrue(TEXT("High rating confers an advantage"), High > Neutral);
    TestTrue(TEXT("Below-range ratings clamp"), FMath::IsNearlyEqual(
        USportsArcadePlayerAttributes::RatingFactor(-100), Low));
    TestTrue(TEXT("Above-range ratings clamp"), FMath::IsNearlyEqual(
        USportsArcadePlayerAttributes::RatingFactor(1000), High));
    for (int32 Rating = 2; Rating <= 99; ++Rating)
    {
        if (!TestTrue(TEXT("Rating factor must be monotonic"),
            USportsArcadePlayerAttributes::RatingFactor(Rating) >=
            USportsArcadePlayerAttributes::RatingFactor(Rating - 1)))
            return false;
    }
    return true;
}

#endif
