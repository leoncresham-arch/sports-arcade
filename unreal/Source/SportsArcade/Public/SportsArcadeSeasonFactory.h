#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SportsArcadeSeasonFactory.generated.h"

class USportsArcadeFootballDatabase;
class USportsArcadeSeasonSave;
class USportsArcadeCupSave;

/** Offline league-mode selection from an authored competition catalog. */
UCLASS()
class SPORTSARCADE_API USportsArcadeSeasonFactory : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /** Creates a season only for valid league competitions and participating clubs. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Season", meta=(WorldContext="WorldContextObject"))
    static USportsArcadeSeasonSave* CreateLeagueSeason(
        const UObject* WorldContextObject,
        const USportsArcadeFootballDatabase* Catalog,
        FName CompetitionId,
        bool bHomeAndAway);

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup", meta=(WorldContext="WorldContextObject"))
    static USportsArcadeCupSave* CreateKnockoutCup(
        const UObject* WorldContextObject,
        const USportsArcadeFootballDatabase* Catalog,
        FName CompetitionId);
};
