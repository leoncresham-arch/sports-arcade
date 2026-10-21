#include "SportsArcadeBall.h"
#include "SportsArcadeCharacter.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ASportsArcadeBall::ASportsArcadeBall()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BallCollision"));
    RootComponent = CollisionSphere;
    CollisionSphere->InitSphereRadius(BallRadiusCm);
    CollisionSphere->SetCollisionProfileName(TEXT("PhysicsActor"));
    CollisionSphere->SetSimulatePhysics(true);
    CollisionSphere->SetEnableGravity(true);
    CollisionSphere->SetNotifyRigidBodyCollision(true);
    CollisionSphere->SetMassOverrideInKg(NAME_None, BallMassKg, true);
    CollisionSphere->BodyInstance.bUseCCD = true;

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallVisual"));
    VisualMesh->SetupAttachment(CollisionSphere);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    VisualMesh->SetCastShadow(true);

    // Primitive reference mesh only. Replace with a properly authored football.
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (Sphere.Succeeded())
    {
        VisualMesh->SetStaticMesh(Sphere.Object);
        VisualMesh->SetRelativeScale3D(FVector(BallRadiusCm / 50.f));
    }
}

void ASportsArcadeBall::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    if (CollisionSphere)
    {
        CollisionSphere->SetSphereRadius(BallRadiusCm);
        CollisionSphere->SetMassOverrideInKg(NAME_None, BallMassKg, true);
    }
    if (VisualMesh)
    {
        VisualMesh->SetRelativeScale3D(FVector(BallRadiusCm / 50.f));
    }
}

void ASportsArcadeBall::BeginPlay()
{
    Super::BeginPlay();
    InitialSpawnLocation = GetActorLocation();
    LastTouchPlayer.Reset();
}

void ASportsArcadeBall::Kick(const FVector& Direction, float StrengthKgCmPerSec)
{
    if (!CollisionSphere || !CollisionSphere->IsSimulatingPhysics() ||
        Direction.ContainsNaN() || Direction.IsNearlyZero() ||
        CollisionSphere->GetPhysicsLinearVelocity().ContainsNaN() ||
        CollisionSphere->GetPhysicsAngularVelocityInDegrees().ContainsNaN() ||
        !FMath::IsFinite(StrengthKgCmPerSec) || StrengthKgCmPerSec <= 0.f)
    {
        return;
    }

    CollisionSphere->AddImpulse(Direction.GetSafeNormal() * StrengthKgCmPerSec);
}

bool ASportsArcadeBall::KickByPlayer(ASportsArcadeCharacter* Player, const FVector& Direction, float StrengthKgCmPerSec, float MaxContactDistanceCm)
{
    if (!IsValid(Player) || Player->GetWorld() != GetWorld() || !CollisionSphere || !CollisionSphere->IsSimulatingPhysics() ||
        !FMath::IsFinite(StrengthKgCmPerSec) || StrengthKgCmPerSec <= 0.f ||
        Direction.ContainsNaN() || Direction.IsNearlyZero() ||
        Player->GetActorLocation().ContainsNaN() || GetActorLocation().ContainsNaN() ||
        CollisionSphere->GetPhysicsLinearVelocity().ContainsNaN() ||
        CollisionSphere->GetPhysicsAngularVelocityInDegrees().ContainsNaN() ||
        !FMath::IsFinite(MaxContactDistanceCm) || MaxContactDistanceCm <= 0.f ||
        FVector::DistSquared(Player->GetActorLocation(), GetActorLocation()) > FMath::Square(MaxContactDistanceCm))
    {
        return false;
    }
    // Attribute the touch only after a finite, nonzero physics impulse.
    if (CollisionSphere->GetPhysicsLinearVelocity().ContainsNaN()) return false;
    Kick(Direction, StrengthKgCmPerSec);
    LastTouchPlayer = Player;
    return true;
}

void ASportsArcadeBall::StopMotion()
{
    if (!CollisionSphere) return;
    CollisionSphere->SetPhysicsLinearVelocity(FVector::ZeroVector);
    CollisionSphere->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
}

void ASportsArcadeBall::PlaceAtRest(const FVector& WorldPosition)
{
    if (!CollisionSphere || WorldPosition.ContainsNaN()) return;
    SetActorLocation(WorldPosition, false, nullptr, ETeleportType::TeleportPhysics);
    StopMotion();
    LastTouchPlayer.Reset();
}

void ASportsArcadeBall::ResetToCentre()
{
    if (!CollisionSphere || InitialSpawnLocation.ContainsNaN())
    {
        return;
    }
    SetActorLocation(InitialSpawnLocation, false, nullptr, ETeleportType::TeleportPhysics);
    StopMotion();
    LastTouchPlayer.Reset();
}

bool ASportsArcadeBall::ApplyControlledTouch(ASportsArcadeCharacter* Player, const FVector& DesiredVelocity, float MaxDistanceCm)
{
    if (!IsValid(Player) || Player->GetWorld() != GetWorld() || !CollisionSphere || !CollisionSphere->IsSimulatingPhysics() ||
        !FMath::IsFinite(MaxDistanceCm) || MaxDistanceCm <= 0.f ||
        DesiredVelocity.ContainsNaN() ||
        Player->GetActorLocation().ContainsNaN() || GetActorLocation().ContainsNaN() ||
        CollisionSphere->GetPhysicsLinearVelocity().ContainsNaN() ||
        CollisionSphere->GetPhysicsAngularVelocityInDegrees().ContainsNaN() ||
        FVector::DistSquared(Player->GetActorLocation(), GetActorLocation()) > FMath::Square(MaxDistanceCm))
    {
        return false;
    }
    // Reject airborne balls and contacts far above/below the player's feet.
    if (FMath::Abs(Player->GetActorLocation().Z - GetActorLocation().Z) > 115.f)
    {
        return false;
    }
    const FVector HorizontalTarget(DesiredVelocity.X, DesiredVelocity.Y, 0.f);
    const FVector Current = CollisionSphere->GetPhysicsLinearVelocity();
    const FVector Correction = HorizontalTarget - FVector(Current.X, Current.Y, 0.f);
    // A gentle ball needs less impulse than a fast incoming pass, but a
    // controlled touch cannot instantly reverse a full-speed football.
    const float IncomingSpeed = FVector(Current.X, Current.Y, 0.f).Size();
    const float TouchDeltaCap = FMath::Lerp(180.f, 280.f,
        FMath::Clamp(IncomingSpeed / 1000.f, 0.f, 1.f));
    const FVector TouchDelta = Correction.GetClampedToMaxSize(TouchDeltaCap);
    if (TouchDelta.ContainsNaN()) return false;
    // Do not award a new possession touch for a negligible/no-op impulse.
    // This protects last-touch ownership from stationary repeated input.
    if (TouchDelta.SizeSquared2D() < FMath::Square(1.f)) return false;
    CollisionSphere->AddImpulse(TouchDelta * BallMassKg);
    LastTouchPlayer = Player;
    return true;
}

int32 ASportsArcadeBall::GetLastTouchTeamIndex() const
{
    const ASportsArcadeCharacter* Player = LastTouchPlayer.Get();
    return IsValid(Player) && Player->GetWorld() == GetWorld() &&
        (Player->TeamIndex == 0 || Player->TeamIndex == 1)
        ? Player->TeamIndex : INDEX_NONE;
}
