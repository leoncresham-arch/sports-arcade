#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SportsArcadeCharacter.generated.h"

UENUM(BlueprintType)
enum class ESportsArcadePosition : uint8
{
    Goalkeeper, LeftBack, CentreBack, RightBack, DefensiveMidfielder,
    CentralMidfielder, AttackingMidfielder, LeftWinger, RightWinger, Striker
};

class USportsArcadePlayerAttributes;
class UCameraComponent;
class USpringArmComponent;

/**
 * Test locomotion and camera scaffold, deliberately asset-agnostic.
 * Requires a rigged skeletal mesh and animation blueprint before visual review.
 */
UCLASS()
class SPORTSARCADE_API ASportsArcadeCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ASportsArcadeCharacter();
    virtual void Tick(float DeltaSeconds) override;
    UFUNCTION(BlueprintPure, Category="Sports Arcade|Movement")
    float GetStaminaNormalized() const;
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Match")
    void ResetToKickoffPosition();

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Movement")
    bool IsJockeying() const { return bJockeyHeld; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Team")
    int32 TeamIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Team")
    int32 ShirtNumber = 1;

    /** Optional stable identity assigned by an authored football database. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Team")
    FName FootballerRecordId;

    /** Zero-based formation slot; 0 goalkeeper, 1-10 outfield. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Team", meta=(ClampMin="0", ClampMax="10"))
    int32 LineupSlot = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Team")
    ESportsArcadePosition FieldPosition = ESportsArcadePosition::CentralMidfielder;

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
    UPROPERTY(VisibleAnywhere, Category="Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, Category="Camera")
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(VisibleAnywhere, Category="Sports Arcade|Ratings")
    TObjectPtr<USportsArcadePlayerAttributes> PlayerAttributes;

    FTransform InitialKickoffTransform = FTransform::Identity;
    bool bSprintHeld = false;
    bool bSprintExhausted = false;
    bool bJockeyHeld = false;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Sports Arcade|Movement", meta=(AllowPrivateAccess="true"))
    float CurrentStamina = 100.f;
    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Movement", meta=(ClampMin="1.0"))
    float MaxStamina = 100.f;
    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Movement", meta=(ClampMin="0.0"))
    float SprintDrainPerSecond = 18.f;
    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Movement", meta=(ClampMin="0.0"))
    float StaminaRecoveryPerSecond = 12.f;
    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Movement", meta=(ClampMin="0.0"))
    float StaminaToResumeSprint = 15.f;
    void BeginSprint();
    void SwitchPlayer();
    void ResumeMatchAfterHalfTime();
    void RestartExhibitionMatch();
    void RestartAfterBallOut();
    void TakeStagedRestart();
    void AwardRestartToHome();
    void AwardRestartToAway();
    void PassBall();
    void ShootBall();
    void ChipBall();
    void TackleBall();
    void BeginJockey();
    void EndJockey();
    float TackleCooldownRemaining = 0.f;
    float StrikeCooldownRemaining = 0.f;
    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Football", meta=(ClampMin="0.0"))
    float StrikeCooldownSeconds = 0.28f;
    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Football", meta=(ClampMin="0.0"))
    float TackleCooldownSeconds = 0.85f;
    float TouchCooldownRemaining = 0.f;
    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Football", meta=(ClampMin="0.0"))
    float DribbleTouchInterval = 0.18f;
    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Football", meta=(ClampMin="0.0"))
    float DribbleTouchRangeCm = 135.f;
    void StrikeNearbyBall(float ImpulseStrength, bool bIsShot, float LiftRatio = 0.f);
    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Football", meta=(ClampMin="0.0", ClampMax="1.0"))
    float ShotLiftRatio = 0.16f;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Football", meta=(ClampMin="0.0", ClampMax="2.0"))
    float ChipLiftRatio = 0.85f;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Football", meta=(ClampMin="0.1", ClampMax="1.0"))
    float ChipPowerFactor = 0.60f;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Football", meta=(ClampMin="0.0"))
    float BallControlReachCm = 170.f;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Football", meta=(ClampMin="0.0"))
    float PassImpulseKgCmPerSec = 1400.f;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Football", meta=(ClampMin="0.0"))
    float ShotImpulseKgCmPerSec = 3200.f;

    void EndSprint();

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Movement", meta=(ClampMin="0"))
    float JogSpeed = 420.f;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Movement", meta=(ClampMin="0"))
    float SprintSpeed = 650.f;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Movement", meta=(ClampMin="0.0"))
    float JockeySpeed = 275.f;

    void MoveForward(float Value);
    void MoveRight(float Value);
};
