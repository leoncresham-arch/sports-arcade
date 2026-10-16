#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "SportsArcadeTeamAIController.generated.h"

/** Lightweight off-ball formation movement for authored roster actors. */
UCLASS()
class SPORTSARCADE_API ASportsArcadeTeamAIController : public AAIController
{
    GENERATED_BODY()
public:
    ASportsArcadeTeamAIController();
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float DecisionIntervalSeconds = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float FormationAdvanceFactor = 0.25f;

private:
    float DecisionCountdown = 0.f;
};
