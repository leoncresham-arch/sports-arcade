#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SportsArcadePlayerAttributes.generated.h"

/** Original 1-99 player ratings; default values are neutral rather than licensed player data. */
UCLASS(ClassGroup=(SportsArcade), meta=(BlueprintSpawnableComponent))
class SPORTSARCADE_API USportsArcadePlayerAttributes : public UActorComponent
{
    GENERATED_BODY()
public:
    USportsArcadePlayerAttributes();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Ratings", meta=(ClampMin="1", ClampMax="99"))
    int32 Acceleration = 65;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Ratings", meta=(ClampMin="1", ClampMax="99"))
    int32 SprintSpeed = 65;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Ratings", meta=(ClampMin="1", ClampMax="99"))
    int32 Stamina = 65;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Ratings", meta=(ClampMin="1", ClampMax="99"))
    int32 ShortPassing = 65;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Ratings", meta=(ClampMin="1", ClampMax="99"))
    int32 Finishing = 65;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Ratings", meta=(ClampMin="1", ClampMax="99"))
    int32 BallControl = 65;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Ratings", meta=(ClampMin="1", ClampMax="99"))
    int32 Tackling = 65;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Ratings")
    static float RatingFactor(int32 Rating);
};
