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

    /** Small forward support shift when our side most recently touched the ball. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Shape", meta=(ClampMin="0.0", ClampMax="500.0"))
    float AttackingSupportShiftCm = 220.f;

    /** Conservative retreat when the opposition was last credited with touch. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Shape", meta=(ClampMin="0.0", ClampMax="500.0"))
    float DefensiveRecoveryShiftCm = 150.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float PressureRadiusCm = 2100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float GoalkeeperLateralRangeCm = 280.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float GoalkeeperAdvanceCm = 350.f;

    /** Reject inaccurate goalkeeping pursuits on balls already moving away. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Goalkeeper", meta=(ClampMin="0.0"))
    float GoalkeeperShotMinSpeedCmPerSec = 175.f;

    /** Highest provisional reachable shot above goalkeeper feet (cm). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Goalkeeper", meta=(ClampMin="0.0"))
    float GoalkeeperParryHeightCm = 220.f;

    /** How far in front of the keeper an incoming ball may be for a parry. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Goalkeeper", meta=(ClampMin="0.0"))
    float GoalkeeperParryOverrunAllowanceCm = 35.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI", meta=(ClampMin="0.0", ClampMax="1.0"))
    float GoalkeeperAnticipationSeconds = 0.35f;

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
