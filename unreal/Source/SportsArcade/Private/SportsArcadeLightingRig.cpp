#include "SportsArcadeLightingRig.h"

#include "Components/SceneComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/ExponentialHeightFogComponent.h"

ASportsArcadeLightingRig::ASportsArcadeLightingRig()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("LightingRoot"));
    SetRootComponent(SceneRoot);

    Sun = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Sunlight"));
    Sun->SetupAttachment(SceneRoot);
    Sun->SetMobility(EComponentMobility::Movable);
    Sun->SetAtmosphereSunLight(true);
    Sun->SetCastShadows(true);

    Sky = CreateDefaultSubobject<USkyLightComponent>(TEXT("AmbientSkylight"));
    Sky->SetupAttachment(SceneRoot);
    Sky->SetMobility(EComponentMobility::Movable);
    Sky->bRealTimeCapture = true;

    Atmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
    Atmosphere->SetupAttachment(SceneRoot);

    Fog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("AtmosphericFog"));
    Fog->SetupAttachment(SceneRoot);
    Fog->FogDensity = 0.003f;
}

void ASportsArcadeLightingRig::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    Sun->SetRelativeRotation(SunDirection);
    // Lighting units are engine-dependent: profile and validate actual
    // exposure / sunlight appearance in Unreal before final art reviews.
    Sun->SetIntensity(KeyLightLux);
    Sky->SetIntensity(SkylightIntensity);
}
