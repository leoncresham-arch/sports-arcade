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

    /** Validate saved fixture identifiers and each recorded result before saving. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Season")
    bool ValidateProgress() const;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Season")
    bool GetCurrentStandings(TArray<FSportsArcadeLeagueStanding>& OutStandings) const;

    /** Returns INDEX_NONE when every fixture has a result. */
    UFUNCTION(BlueprintPure, Category="Sports Arcade|Season")
    int32 GetNextUnplayedFixtureIndex() const;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Season")
    bool IsSeasonComplete() const;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Season")
    int32 GetCompletedFixtureCount() const;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Season")
    float GetSeasonProgress() const;

    /** Persist this season using Unreal's standard local save-slot system. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Season|Save")
    bool SaveSeasonToSlot(const FString& SlotName, int32 UserIndex = 0) const;

    /** Returns nullptr for a missing slot or incompatible save-game class. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Season|Save", meta=(WorldContext="WorldContextObject"))
    static USportsArcadeSeasonSave* LoadSeasonFromSlot(const UObject* WorldContextObject,
        const FString& SlotName, int32 UserIndex = 0);
};
