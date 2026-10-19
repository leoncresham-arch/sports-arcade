#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SportsArcadeFixtures.generated.h"

USTRUCT(BlueprintType)
struct FSportsArcadeLeagueFixture
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Season")
    int32 Round = 0;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Season")
    FName HomeClubId;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Season")
    FName AwayClubId;
};

/** Pure deterministic offline round-robin scheduling, independent of match actors. */
UCLASS()
class SPORTSARCADE_API USportsArcadeFixtures : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Return false for duplicate/empty ids or fewer than two clubs; odd leagues receive byes. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Season")
    static bool GenerateLeagueFixtures(const TArray<FName>& ClubIds, bool bHomeAndAway,
        TArray<FSportsArcadeLeagueFixture>& Fixtures);
};
