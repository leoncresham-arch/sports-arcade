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
        Direction.IsNearlyZero() || StrengthKgCmPerSec <= 0.f)
    {
        return;
    }

    CollisionSphere->AddImpulse(Direction.GetSafeNormal() * StrengthKgCmPerSec);
}

bool ASportsArcadeBall::KickByPlayer(ASportsArcadeCharacter* Player, const FVector& Direction, float StrengthKgCmPerSec, float MaxContactDistanceCm)
{
    if (!IsValid(Player) || !CollisionSphere || !CollisionSphere->IsSimulatingPhysics() ||
        StrengthKgCmPerSec <= 0.f || Direction.IsNearlyZero() || MaxContactDistanceCm <= 0.f ||
        FVector::DistSquared(Player->GetActorLocation(), GetActorLocation()) > FMath::Square(MaxContactDistanceCm))
    {
        return false;
    }
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
    if (!CollisionSphere) return;
    StopMotion();
    LastTouchPlayer.Reset();
    SetActorLocation(WorldPosition, false, nullptr, ETeleportType::TeleportPhysics);
}

void ASportsArcadeBall::ResetToCentre()
{
    if (!CollisionSphere)
    {
        return;
    }
    LastTouchPlayer.Reset();
    StopMotion();
    SetActorLocation(InitialSpawnLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

bool ASportsArcadeBall::ApplyControlledTouch(ASportsArcadeCharacter* Player, const FVector& DesiredVelocity, float MaxDistanceCm)
{
    if (!IsValid(Player) || !CollisionSphere || !CollisionSphere->IsSimulatingPhysics() ||
        MaxDistanceCm <= 0.f || FVector::DistSquared(Player->GetActorLocation(), GetActorLocation()) > FMath::Square(MaxDistanceCm))
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
    // Controlled touches accelerate the rolling ball; they do not teleport it.
    CollisionSphere->AddImpulse(Correction.GetClampedToMaxSize(280.f) * BallMassKg);
    LastTouchPlayer = Player;
    return true;
}

int32 ASportsArcadeBall::GetLastTouchTeamIndex() const
{
    const ASportsArcadeCharacter* Player = LastTouchPlayer.Get();
    return IsValid(Player) && (Player->TeamIndex == 0 || Player->TeamIndex == 1)
        ? Player->TeamIndex : INDEX_NONE;
}
