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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float PressureRadiusCm = 2100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float GoalkeeperLateralRangeCm = 280.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float GoalkeeperAdvanceCm = 350.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float BallPredictionSeconds = 0.20f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float ContactRangeCm = 145.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float MarkingSearchRadiusCm = 950.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float MarkingInfluence = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float ActionCooldownSeconds = 0.9f;

private:
    float DecisionCountdown = 0.f;
    float ActionCooldownRemaining = 0.f;
};
