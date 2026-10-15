#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "SportsArcadeBroadcastCamera.generated.h"

/**
 * Configurable broadcast camera for the PC football vertical slice.
 * All distances are in Unreal units (centimeters).
 *
 * Place in an authored stadium level and set as PlayerController view target.
 * The preview camera is NOT a replacement for a licensed stadium or player asset.
 */
UCLASS(Blueprintable)
class SPORTSARCADE_API ASportsArcadeBroadcastCamera : public ACameraActor
{
    GENERATED_BODY()

public:
    ASportsArcadeBroadcastCamera();

    virtual void Tick(float DeltaSeconds) override;

    /** Track the active ball or a manually positioned focus target. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Camera")
    void SetFocusActor(AActor* NewFocusActor);

    /** Clear the tracked actor; camera returns smoothly to the centre of the pitch. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Camera")
    void ClearFocusActor();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Camera", meta=(ClampMin="0.1"))
    float TrackingSpeed = 2.2f;

    /** Limits lateral camera travel along the touchline, measured in cm. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Camera", meta=(ClampMin="0.0"))
    float MaximumLateralTravel = 1800.f;

    /** Position in a level whose origin is the centre spot. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Camera")
    FVector PitchCentre = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Camera", meta=(ClampMin="1.0", ClampMax="170.0"))
    float BroadcastFieldOfView = 48.f;

private:
    UPROPERTY(Transient)
    TObjectPtr<AActor> FocusActor;

    /** Offset from centre spot to a touchline camera, looking across the pitch. */
    FVector CameraOffset = FVector(0.f, -8900.f, 3900.f);
};
