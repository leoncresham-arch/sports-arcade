#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SportsArcadeGameMode.generated.h"

/** PC-first technical foundation. Match rules will be implemented after art vertical slice. */
UCLASS()
class SPORTSARCADE_API ASportsArcadeGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ASportsArcadeGameMode();

    /** Use the placed TV camera in a visual review level when one exists. */
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Visual Review")
    bool bUseBroadcastCameraIfPresent = true;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Visual Review")
    bool bTrackReferenceFootball = true;
};
