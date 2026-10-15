#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SportsArcadeAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;

/**
 * Animation data bridge for a realistic footballer rig.
 *
 * Attach a Blueprint derived from this class to a licensed skeletal mesh,
 * then drive an authored Blend Space, transitions and motion-matching assets.
 * It deliberately ships with no animations or claimed photorealistic preview.
 */
UCLASS(Blueprintable, BlueprintType)
class SPORTSARCADE_API USportsArcadeAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Locomotion")
    float GroundSpeed = 0.f;

    /** Signed horizontal movement angle relative to character forward, degrees. */
    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Locomotion")
    float MovementDirection = 0.f;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Locomotion")
    float TurnRateDegrees = 0.f;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Locomotion")
    float NormalizedAcceleration = 0.f;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Locomotion")
    bool bIsMoving = false;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Locomotion")
    bool bIsFalling = false;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Locomotion")
    bool bIsSprinting = false;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sports Arcade|Locomotion")
    float SprintSpeedThreshold = 560.f;

private:
    UPROPERTY(Transient)
    TObjectPtr<ACharacter> CachedCharacter;

    UPROPERTY(Transient)
    TObjectPtr<UCharacterMovementComponent> CachedMovement;

    float PreviousYaw = 0.f;
    bool bHasPreviousYaw = false;
};
