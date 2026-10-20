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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Attack", meta=(ClampMin="0.0", ClampMax="1500.0"))
    float ForwardRunOffsetCm = 650.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Attack", meta=(ClampMin="0.0", ClampMax="800.0"))
    float ForwardRunLaneSeparationCm = 250.f;

    /** Conservative retreat when the opposition was last credited with touch. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Shape", meta=(ClampMin="0.0", ClampMax="500.0"))
    float DefensiveRecoveryShiftCm = 150.f;
    
    /** Keep the defensive back line from following the ball too far upfield. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Defence", meta=(ClampMin="0.0", ClampMax="2500.0"))
    float DefensiveLineMaximumAdvanceCm = 1300.f;

    /** Limit how far the defensive line drops toward its own goal. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Defence", meta=(ClampMin="0.0", ClampMax="2000.0"))
    float DefensiveLineRecoveryDepthCm = 650.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Defence", meta=(ClampMin="0.0", ClampMax="1500.0"))
    float DefensiveLineLateralDisplacementCm = 650.f;
    
    /** Maximum distance for an AI teammate to occupy a lateral passing lane. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Shape", meta=(ClampMin="0.0", ClampMax="1200.0"))
    float PassingSupportLateralShiftCm = 350.f;

    /** Opponent proximity used to choose the less congested supporting side. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Shape", meta=(ClampMin="0.0", ClampMax="2000.0"))
    float PassingSupportOpponentSearchCm = 1000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float PressureRadiusCm = 2100.f;
    
    /** Closest additional defender shadows the ball when the opposition last touched it. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Defence", meta=(ClampMin="0.0", ClampMax="3000.0"))
    float SecondaryPressureRadiusCm = 1700.f;

    /** Keep the supporting defender behind the lead presser, not on the same spot. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Defence", meta=(ClampMin="0.0", ClampMax="1000.0"))
    float SecondaryPressureStandOffCm = 450.f;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Pressing", meta=(ClampMin="100.0", ClampMax="1000.0"))
    float PresserTravelSpeedCmPerSec = 500.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float ContactRangeCm = 145.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float MarkingSearchRadiusCm = 950.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float MarkingInfluence = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI")
    float ActionCooldownSeconds = 0.9f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|AI|Dribbling", meta=(ClampMin="50.0", ClampMax="400.0"))
    float DribbleChallengeRadiusCm = 150.f;

private:
    float DecisionCountdown = 0.f;
    float ActionCooldownRemaining = 0.f;
};
