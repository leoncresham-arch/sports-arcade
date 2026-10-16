#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SportsArcadeBall.generated.h"

class ASportsArcadeCharacter;
class USphereComponent;
class UStaticMeshComponent;
class UMaterialInterface;

/**
 * Native ball-physics starting point (not a finished football simulation).
 * Centimetres, kilograms and UE Chaos physics. Requires editor validation.
 */
UCLASS(Blueprintable)
class SPORTSARCADE_API ASportsArcadeBall : public AActor
{
    GENERATED_BODY()

public:
    ASportsArcadeBall();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Sports Arcade|Ball")
    TObjectPtr<USphereComponent> CollisionSphere;

    /** Optional licensed/authored mesh; sphere collision drives ball physics. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Sports Arcade|Ball")
    TObjectPtr<UStaticMeshComponent> VisualMesh;

    /** 22 cm diameter football: 11 cm radius. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sports Arcade|Ball",
              meta=(ClampMin="10.5", ClampMax="11.5"))
    float BallRadiusCm = 11.f;

    /** Regulation size 5 football range is approximately 410-450 g. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sports Arcade|Ball",
              meta=(ClampMin="0.41", ClampMax="0.45"))
    float BallMassKg = 0.43f;

    /** Apply a directional impulse in world space; impulse units are kg*cm/s. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Ball")
    void Kick(const FVector& Direction, float StrengthKgCmPerSec);

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Ball")
    bool KickByPlayer(ASportsArcadeCharacter* Player, const FVector& Direction, float StrengthKgCmPerSec, float MaxContactDistanceCm);

    /** Physics-based controlled touch, never attaches the ball to the player. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Ball")
    bool ApplyControlledTouch(ASportsArcadeCharacter* Player, const FVector& DesiredVelocity, float MaxDistanceCm);

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Ball")
    ASportsArcadeCharacter* GetLastTouchPlayer() const { return LastTouchPlayer.Get(); }

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Ball")
    int32 GetLastTouchTeamIndex() const;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Ball")
    void ResetToCentre();

    /** Zeroes ball motion without altering its position or touch history. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Ball")
    void StopMotion();

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Ball")
    void PlaceAtRest(const FVector& WorldPosition);

    virtual void OnConstruction(const FTransform& Transform) override;

private:
    FVector InitialSpawnLocation = FVector::ZeroVector;
    TWeakObjectPtr<ASportsArcadeCharacter> LastTouchPlayer;
    virtual void BeginPlay() override;
};
