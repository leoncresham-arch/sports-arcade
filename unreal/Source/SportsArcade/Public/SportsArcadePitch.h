#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SportsArcadePitch.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
class UStaticMeshComponent;
class USceneComponent;
class UMaterialInterface;

/**
 * Accurate football pitch foundation; geometry is in Unreal centimetres.
 * The generated cube geometry is only a UV/placement scaffold: realistic turf,
 * paint and stadium meshes must be authored and assigned separately.
 */
UCLASS(Blueprintable)
class SPORTSARCADE_API ASportsArcadePitch : public AActor
{
    GENERATED_BODY()

public:
    ASportsArcadePitch();
    virtual void OnConstruction(const FTransform& Transform) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Pitch", meta=(ClampMin="9000.0", ClampMax="12000.0"))
    float LengthCm = 10500.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Pitch", meta=(ClampMin="4500.0", ClampMax="9000.0"))
    float WidthCm = 6800.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Pitch")
    TObjectPtr<UMaterialInterface> TurfMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Pitch")
    TObjectPtr<UMaterialInterface> MarkingMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Pitch", meta=(ClampMin="1", ClampMax="15"))
    float LineWidthCm = 12.f;

private:
    UPROPERTY(VisibleAnywhere, Category="Sports Arcade|Pitch")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, Category="Sports Arcade|Pitch")
    TObjectPtr<UStaticMeshComponent> Surface;

    UPROPERTY(VisibleAnywhere, Category="Sports Arcade|Pitch")
    TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Lines;

    void AddSegment(const FVector2D& Start, const FVector2D& End);
    void AddRectangle(float X0, float Y0, float X1, float Y1);
    void AddCircle(float X, float Y, float Radius, int32 Segments);
};
