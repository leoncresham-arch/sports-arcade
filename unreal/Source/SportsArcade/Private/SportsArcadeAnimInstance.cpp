#include "SportsArcadeAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void USportsArcadeAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    CachedCharacter = Cast<ACharacter>(TryGetPawnOwner());
    CachedMovement = CachedCharacter ? CachedCharacter->GetCharacterMovement() : nullptr;
    bHasPreviousYaw = false;
}

void USportsArcadeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // The mesh can change owner during editor preview or character respawn.
    if (!IsValid(CachedCharacter))
    {
        CachedCharacter = Cast<ACharacter>(TryGetPawnOwner());
        CachedMovement = CachedCharacter ? CachedCharacter->GetCharacterMovement() : nullptr;
        bHasPreviousYaw = false;
    }

    if (!IsValid(CachedCharacter) || !IsValid(CachedMovement))
    {
        GroundSpeed = 0.f;
        MovementDirection = 0.f;
        TurnRateDegrees = 0.f;
        NormalizedAcceleration = 0.f;
        bIsMoving = false;
        bIsFalling = false;
        bIsSprinting = false;
        return;
    }

    const FVector Velocity = CachedCharacter->GetVelocity();
    const FVector HorizontalVelocity(Velocity.X, Velocity.Y, 0.f);
    GroundSpeed = HorizontalVelocity.Size();
    bIsMoving = GroundSpeed > 8.f;
    bIsFalling = CachedMovement->IsFalling();

    const FRotator ActorRotation = CachedCharacter->GetActorRotation();
    const FVector LocalVelocity = ActorRotation.UnrotateVector(HorizontalVelocity);
    MovementDirection = bIsMoving
        ? FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X))
        : 0.f;

    const float CurrentYaw = ActorRotation.Yaw;
    TurnRateDegrees = (DeltaSeconds > KINDA_SMALL_NUMBER && bHasPreviousYaw)
        ? FMath::FindDeltaAngleDegrees(PreviousYaw, CurrentYaw) / DeltaSeconds
        : 0.f;
    PreviousYaw = CurrentYaw;
    bHasPreviousYaw = true;

    const float MaxAcceleration = FMath::Max(CachedMovement->GetMaxAcceleration(), 1.f);
    NormalizedAcceleration = FMath::Clamp(
        CachedMovement->GetCurrentAcceleration().Size() / MaxAcceleration, 0.f, 1.f);

    bIsSprinting = bIsMoving && GroundSpeed >= SprintSpeedThreshold;
}
