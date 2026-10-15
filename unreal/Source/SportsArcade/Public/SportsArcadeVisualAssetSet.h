#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UObject/SoftObjectPtr.h"
#include "SportsArcadeVisualAssetSet.generated.h"

class USkeletalMesh;
class UAnimSequence;
class UTexture2D;
class UMaterialInterface;
class UStaticMesh;

/**
 * Licensed content references for a premium football visual vertical slice.
 * Soft references keep third-party content out of source-only modules.
 * Create a Data Asset in Unreal; import/license-check each actual asset separately.
 */
UCLASS(BlueprintType)
class SPORTSARCADE_API USportsArcadeVisualAssetSet : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
    TSoftObjectPtr<USkeletalMesh> PlayerMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
    TSoftObjectPtr<USkeletalMesh> GoalkeeperMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
    TSoftObjectPtr<UMaterialInterface> HomeKitMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
    TSoftObjectPtr<UMaterialInterface> AwayKitMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
    TSoftObjectPtr<UAnimSequence> IdleAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
    TSoftObjectPtr<UAnimSequence> JogAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
    TSoftObjectPtr<UAnimSequence> SprintAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pitch")
    TSoftObjectPtr<UMaterialInterface> TurfMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pitch")
    TSoftObjectPtr<UStaticMesh> GoalFrameMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pitch")
    TSoftObjectPtr<UMaterialInterface> GoalNetMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stadium")
    TSoftObjectPtr<UStaticMesh> StadiumMesh;

    /** Freeform asset source description for traceability; not a license grant. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Licensing")
    FString SourceAndLicenseNotes;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Assets")
    bool HasCoreVisualReferences() const;
};
