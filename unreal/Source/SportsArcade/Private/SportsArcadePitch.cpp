#include "SportsArcadePitch.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "UObject/ConstructorHelpers.h"

ASportsArcadePitch::ASportsArcadePitch()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PitchRoot"));
    RootComponent = SceneRoot;

    Surface = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurfSurface"));
    Surface->SetupAttachment(SceneRoot);
    Surface->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    Lines = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("FieldMarkings"));
    Lines->SetupAttachment(SceneRoot);
    Lines->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Lines->SetCastShadow(false);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (Cube.Succeeded())
    {
        Surface->SetStaticMesh(Cube.Object);
        Lines->SetStaticMesh(Cube.Object);
    }
}

void ASportsArcadePitch::AddSegment(const FVector2D& Start, const FVector2D& End)
{
    const FVector2D Difference = End - Start;
    const float Distance = Difference.Size();
    if (Distance < 0.1f)
    {
        return;
    }
    const FVector2D Middle = (Start + End) * 0.5f;
    const float Rotation = FMath::RadiansToDegrees(FMath::Atan2(Difference.Y, Difference.X));

    // Engine cube mesh is 100 cm per axis. Markings float just above the turf.
    const FVector Position(Middle.X, Middle.Y, 0.5f);
    const FVector Size(Distance / 100.f, LineWidthCm / 100.f, 1.f / 100.f);
    Lines->AddInstance(FTransform(FRotator(0.f, Rotation, 0.f), Position, Size));
}

void ASportsArcadePitch::AddRectangle(float X0, float Y0, float X1, float Y1)
{
    AddSegment(FVector2D(X0, Y0), FVector2D(X1, Y0));
    AddSegment(FVector2D(X1, Y0), FVector2D(X1, Y1));
    AddSegment(FVector2D(X1, Y1), FVector2D(X0, Y1));
    AddSegment(FVector2D(X0, Y1), FVector2D(X0, Y0));
}

void ASportsArcadePitch::AddCircle(float X, float Y, float Radius, int32 Segments)
{
    for (int32 Index = 0; Index < Segments; ++Index)
    {
        const float A = UE_TWO_PI * Index / Segments;
        const float B = UE_TWO_PI * (Index + 1) / Segments;
        AddSegment(
            FVector2D(X + Radius * FMath::Cos(A), Y + Radius * FMath::Sin(A)),
            FVector2D(X + Radius * FMath::Cos(B), Y + Radius * FMath::Sin(B)));
    }
}

void ASportsArcadePitch::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    Lines->ClearInstances();
    if (!Surface->GetStaticMesh() || !Lines->GetStaticMesh())
    {
        return;
    }

    // Default cube centred on origin, top face at local Z=0.
    Surface->SetRelativeScale3D(FVector(LengthCm / 100.f, WidthCm / 100.f, 0.2f));
    Surface->SetRelativeLocation(FVector(0.f, 0.f, -10.f));

    if (TurfMaterial)
    {
        Surface->SetMaterial(0, TurfMaterial);
    }
    if (MarkingMaterial)
    {
        Lines->SetMaterial(0, MarkingMaterial);
    }

    const float HalfLength = LengthCm * 0.5f;
    const float HalfWidth = WidthCm * 0.5f;
    AddRectangle(-HalfLength, -HalfWidth, HalfLength, HalfWidth);
    AddSegment(FVector2D(0.f, -HalfWidth), FVector2D(0.f, HalfWidth));
    AddCircle(0.f, 0.f, 915.f, 96);

    // Standard penalty / goal areas; clamp ranges when field dimensions change.
    const float PenaltyHalfWidth = FMath::Min(2016.f, HalfWidth - 200.f);
    const float GoalAreaHalfWidth = FMath::Min(916.f, HalfWidth - 100.f);
    for (const float Side : { -1.f, 1.f })
    {
        const float GoalLineX = Side * HalfLength;
        AddRectangle(
            FMath::Min(GoalLineX, GoalLineX - Side * 1650.f), -PenaltyHalfWidth,
            FMath::Max(GoalLineX, GoalLineX - Side * 1650.f), PenaltyHalfWidth);
        AddRectangle(
            FMath::Min(GoalLineX, GoalLineX - Side * 550.f), -GoalAreaHalfWidth,
            FMath::Max(GoalLineX, GoalLineX - Side * 550.f), GoalAreaHalfWidth);

        // Penalty mark rendered as short white cross, 11m from goal line.
        const float PenaltyX = GoalLineX - Side * 1100.f;
        AddSegment(FVector2D(PenaltyX - 8.f, 0.f), FVector2D(PenaltyX + 8.f, 0.f));
        AddSegment(FVector2D(PenaltyX, -8.f), FVector2D(PenaltyX, 8.f));

        // Penalty arc outside each box (9.15m from penalty mark).
        // Only retain the points on the centre-spot side of the penalty box.
        const int32 Samples = 128;
        for (int32 I = 0; I < Samples; ++I)
        {
            const float A = UE_TWO_PI * I / Samples;
            const float B = UE_TWO_PI * (I + 1) / Samples;
            const FVector2D P0(PenaltyX + 915.f * FMath::Cos(A), 915.f * FMath::Sin(A));
            const FVector2D P1(PenaltyX + 915.f * FMath::Cos(B), 915.f * FMath::Sin(B));
            const float BoxEdge = GoalLineX - Side * 1650.f;
            if ((P0.X - BoxEdge) * (-Side) > 0.f &&
                (P1.X - BoxEdge) * (-Side) > 0.f)
            {
                AddSegment(P0, P1);
            }
        }
    }
}
