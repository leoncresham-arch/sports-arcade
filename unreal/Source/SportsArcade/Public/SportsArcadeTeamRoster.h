#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SportsArcadeTeamRoster.generated.h"

class ASportsArcadeCharacter;

UENUM(BlueprintType)
enum class ESportsArcadeFormation : uint8
{
    Formation433, Formation442
};

/** Level-authored team roster; does not automatically spawn or license players. */
UCLASS(Blueprintable)
class SPORTSARCADE_API ASportsArcadeTeamRoster : public AActor
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Team")
    bool bDiscoverPlacedPlayers = true;

    /** Use each player's authored LineupSlot instead of roster array order. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Team")
    bool bUseExplicitLineupSlots = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Team", meta=(ClampMin="0", ClampMax="1"))
    int32 TeamIndex = 0;

    /** Stable club identifier assigned when a season fixture is selected. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Team")
    FName ClubId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Team")
    ESportsArcadeFormation Formation = ESportsArcadeFormation::Formation433;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Team")
    TArray<TObjectPtr<ASportsArcadeCharacter>> Players;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Team")
    bool AddPlayer(ASportsArcadeCharacter* Player);

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Team")
    void DiscoverPlacedPlayers();

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Team")
    int32 GetPlayerCount() const;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Team")
    bool HasCompleteLineup() const;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Team")
    FVector GetFormationPosition(int32 PlayerSlot) const;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Team")
    ASportsArcadeCharacter* GetClosestPlayer(const FVector& Location) const;
};
