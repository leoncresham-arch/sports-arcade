#include "SportsArcadeGoal.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "UObject/ConstructorHelpers.h"

ASportsArcadeGoal::ASportsArcadeGoal()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GoalRoot"));
    SetRootComponent(SceneRoot);

    FramePieces = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("GuideFrame"));
    FramePieces->SetupAttachment(SceneRoot);
    FramePieces->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (Cube.Succeeded())
    {
        FramePieces->SetStaticMesh(Cube.Object);
    }
}

void ASportsArcadeGoal::AddFramePiece(const FVector& Centre, const FVector& LengthsCm)
{
    // Engine cube dimensions are 100 cm on each axis.
    FramePieces->AddInstance(FTransform(
        FRotator::ZeroRotator, Centre, LengthsCm / 100.f));
}

void ASportsArcadeGoal::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    FramePieces->ClearInstances();
    if (!FramePieces->GetStaticMesh())
    {
        return;
    }

    if (FrameMaterial)
    {
        FramePieces->SetMaterial(0, FrameMaterial);
    }

    const float T = FrameThicknessCm;
    const float HalfInterior = InternalWidthCm * 0.5f;
    const float PostCentreY = HalfInterior + T * 0.5f;
    // Goal plane is local X=0; local Y runs between posts; Z is up.
    const float PostCentreZ = (InternalHeightCm + T * 0.5f) * 0.5f;
    const float FullPostHeight = InternalHeightCm + T * 0.5f;

    AddFramePiece(FVector(0.f, -PostCentreY, PostCentreZ),
                  FVector(T, T, FullPostHeight));
    AddFramePiece(FVector(0.f, PostCentreY, PostCentreZ),
                  FVector(T, T, FullPostHeight));
    AddFramePiece(FVector(0.f, 0.f, InternalHeightCm + T * 0.5f),
                  FVector(T, InternalWidthCm + 2.f * T, T));
}
