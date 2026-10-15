#include "SportsArcadeBroadcastCamera.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

ASportsArcadeBroadcastCamera::ASportsArcadeBroadcastCamera()
{
    PrimaryActorTick.bCanEverTick = true;
    // Do not set world transform in a constructor: the level owns camera placement.
    // Tick performs the eventual tracking; editor setup places the first shot.
    if (UCameraComponent* Camera = GetCameraComponent())
    {
        Camera->SetFieldOfView(BroadcastFieldOfView);
    }
}

void ASportsArcadeBroadcastCamera::SetFocusActor(AActor* NewFocusActor)
{
    FocusActor = NewFocusActor;
}

void ASportsArcadeBroadcastCamera::ClearFocusActor()
{
    FocusActor = nullptr;
}

void ASportsArcadeBroadcastCamera::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    FVector Target = PitchCentre;
    if (IsValid(FocusActor))
    {
        const FVector Position = FocusActor->GetActorLocation();
        Target.X += FMath::Clamp(Position.X - PitchCentre.X,
                                -MaximumLateralTravel, MaximumLateralTravel);
        // A broadcast camera mainly tracks play laterally rather than
        // wildly changing its elevation or touchline position.
    }

    const FVector DesiredLocation = Target + CameraOffset;
    const FVector NextLocation = FMath::VInterpTo(
        GetActorLocation(), DesiredLocation, DeltaSeconds, TrackingSpeed);
    SetActorLocation(NextLocation);
    SetActorRotation(UKismetMathLibrary::FindLookAtRotation(
        NextLocation, Target));

    if (UCameraComponent* Camera = GetCameraComponent())
    {
        Camera->SetFieldOfView(BroadcastFieldOfView);
    }
}
