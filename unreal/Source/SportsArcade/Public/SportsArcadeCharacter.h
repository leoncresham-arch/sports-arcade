#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SportsArcadeCharacter.generated.h"

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

protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
    UPROPERTY(VisibleAnywhere, Category="Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, Category="Camera")
    TObjectPtr<UCameraComponent> FollowCamera;

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
