#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SportsArcadeFixtures.h"
#include "SportsArcadeLeagueTable.h"
#include "SportsArcadeSeasonSave.generated.h"

/** Offline league-season progress; all identifiers are authored database IDs. */
UCLASS(BlueprintType)
class SPORTSARCADE_API USportsArcadeSeasonSave : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Season")
    FName CompetitionId;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Season")
    TArray<FName> ClubIds;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Season")
    TArray<FSportsArcadeLeagueFixture> Fixtures;

    /** Fixed fixture indices, preventing duplicate submissions on save/load. */
    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Season")
    TArray<int32> CompletedFixtureIndices;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Season")
    TArray<FSportsArcadeLeagueResult> Results;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Season")
    bool StartSeason(FName InCompetitionId, const TArray<FName>& InClubIds, bool bHomeAndAway);

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Season")
    bool RecordFixtureResult(int32 FixtureIndex, int32 HomeGoals, int32 AwayGoals);

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Season")
    bool GetCurrentStandings(TArray<FSportsArcadeLeagueStanding>& OutStandings) const;
};
