#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SportsArcadeCupFixtures.generated.h"

USTRUCT(BlueprintType)
struct FSportsArcadeCupFixture
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 Round = 1;

    UPROPERTY(BlueprintReadOnly)
    FName HomeClubId;

    UPROPERTY(BlueprintReadOnly)
    FName AwayClubId;
};

/** Single-elimination cup bracket generator, without proprietary competition data. */
UCLASS()
class SPORTSARCADE_API USportsArcadeCupFixtures : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Starts a cup with a 2/4/8/16/32/64-team first round; later rounds derive from winners. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup")
    static bool GenerateOpeningRound(const TArray<FName>& ClubIds,
        TArray<FSportsArcadeCupFixture>& OutFixtures);

    /** Constructs the next round only when exactly one distinct winner is supplied per match. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup")
    static bool GenerateNextRound(const TArray<FSportsArcadeCupFixture>& PreviousRound,
        const TArray<FName>& Winners, TArray<FSportsArcadeCupFixture>& OutFixtures);
};
