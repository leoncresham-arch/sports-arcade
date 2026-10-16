#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SportsArcadeTeamRoster.generated.h"

class ASportsArcadeCharacter;

/** Level-authored team roster; does not automatically spawn or license players. */
UCLASS(Blueprintable)
class SPORTSARCADE_API ASportsArcadeTeamRoster : public AActor
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Team", meta=(ClampMin="0", ClampMax="1"))
    int32 TeamIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Team")
    TArray<TObjectPtr<ASportsArcadeCharacter>> Players;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Team")
    bool AddPlayer(ASportsArcadeCharacter* Player);

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Team")
    int32 GetPlayerCount() const;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Team")
    ASportsArcadeCharacter* GetClosestPlayer(const FVector& Location) const;
};
