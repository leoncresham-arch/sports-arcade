#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SportsArcadeLightingRig.generated.h"

class USceneComponent;
class UDirectionalLightComponent;
class USkyLightComponent;
class USkyAtmosphereComponent;
class UExponentialHeightFogComponent;

/**
 * Repeatable starting point for daylight stadium visual reviews.
 * This only supplies engine lighting; realistic assets and tuning are required.
 */
UCLASS(Blueprintable)
class SPORTSARCADE_API ASportsArcadeLightingRig : public AActor
{
    GENERATED_BODY()

public:
    ASportsArcadeLightingRig();
    virtual void OnConstruction(const FTransform& Transform) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Lighting",
              meta=(ClampMin="0.0", ClampMax="20.0"))
    float KeyLightLux = 100000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Lighting",
              meta=(ClampMin="0.0", ClampMax="10.0"))
    float SkylightIntensity = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Lighting")
    FRotator SunDirection = FRotator(-38.f, -35.f, 0.f);

private:
    UPROPERTY(VisibleAnywhere, Category="Sports Arcade|Lighting")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, Category="Sports Arcade|Lighting")
    TObjectPtr<UDirectionalLightComponent> Sun;

    UPROPERTY(VisibleAnywhere, Category="Sports Arcade|Lighting")
    TObjectPtr<USkyLightComponent> Sky;

    UPROPERTY(VisibleAnywhere, Category="Sports Arcade|Lighting")
    TObjectPtr<USkyAtmosphereComponent> Atmosphere;

    UPROPERTY(VisibleAnywhere, Category="Sports Arcade|Lighting")
    TObjectPtr<UExponentialHeightFogComponent> Fog;
};
