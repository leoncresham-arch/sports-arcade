#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SportsArcadeLeagueTable.generated.h"

USTRUCT(BlueprintType)
struct FSportsArcadeLeagueResult
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName HomeClubId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AwayClubId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 HomeGoals = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 AwayGoals = 0;
};

USTRUCT(BlueprintType)
struct FSportsArcadeLeagueStanding
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly)
    FName ClubId;
    UPROPERTY(BlueprintReadOnly)
    int32 Played = 0;
    UPROPERTY(BlueprintReadOnly)
    int32 Won = 0;
    UPROPERTY(BlueprintReadOnly)
    int32 Drawn = 0;
    UPROPERTY(BlueprintReadOnly)
    int32 Lost = 0;
    UPROPERTY(BlueprintReadOnly)
    int32 GoalsFor = 0;
    UPROPERTY(BlueprintReadOnly)
    int32 GoalsAgainst = 0;
    UPROPERTY(BlueprintReadOnly)
    int32 Points = 0;
};

/** League table arithmetic is pure and suitable for offline seasonal saves. */
UCLASS()
class SPORTSARCADE_API USportsArcadeLeagueTable : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /** Rejects invalid results; outputs ordered standings by points, goal difference, goals for, stable ID. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Season")
    static bool CalculateTable(const TArray<FName>& ClubIds,
        const TArray<FSportsArcadeLeagueResult>& Results,
        TArray<FSportsArcadeLeagueStanding>& Standings);
};
