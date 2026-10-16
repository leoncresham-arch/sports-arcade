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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Team")
    int32 TeamIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Team")
    int32 ShirtNumber = 1;

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

    bool bSprintHeld = false;
    bool bSprintExhausted = false;
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
    void PassBall();
    void ShootBall();
    void StrikeNearbyBall(float ImpulseStrength);

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

    void MoveForward(float Value);
    void MoveRight(float Value);
};
