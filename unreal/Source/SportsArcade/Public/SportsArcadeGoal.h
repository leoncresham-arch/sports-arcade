#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SportsArcadeGoal.generated.h"

class USceneComponent;
class UInstancedStaticMeshComponent;
class UMaterialInterface;

/**
 * Dimensions-only football goal reference actor for authored stadium levels.
 *
 * The native cube-based frame is a construction guide, NOT a production-quality
 * mesh, net, or goal animation. Units are centimetres (UE default).
 * Face the actor towards the field when placing behind either goal line.
 */
UCLASS(Blueprintable)
class SPORTSARCADE_API ASportsArcadeGoal : public AActor
{
    GENERATED_BODY()

public:
    ASportsArcadeGoal();
    virtual void OnConstruction(const FTransform& Transform) override;

    /** FIFA/IFAB standard interior width: 7.32 m. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Goal",
              meta=(ClampMin="732.0", ClampMax="732.0"))
    float InternalWidthCm = 732.f;

    /** Standard height from ground to underside of crossbar: 2.44 m. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Goal",
              meta=(ClampMin="244.0", ClampMax="244.0"))
    float InternalHeightCm = 244.f;

    /** Frame thickness, for the placement scaffold only. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Goal",
              meta=(ClampMin="8.0", ClampMax="12.0"))
    float FrameThicknessCm = 12.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Goal")
    TObjectPtr<UMaterialInterface> FrameMaterial;

private:
    UPROPERTY(VisibleAnywhere, Category="Sports Arcade|Goal")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, Category="Sports Arcade|Goal")
    TObjectPtr<UInstancedStaticMeshComponent> FramePieces;

    void AddFramePiece(const FVector& Centre, const FVector& LengthsCm);
};
