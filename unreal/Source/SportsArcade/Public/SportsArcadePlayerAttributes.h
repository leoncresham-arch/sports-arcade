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
    /** Responsiveness of planted turns and facing direction changes. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Ratings", meta=(ClampMin="1", ClampMax="99"))
    int32 Agility = 65;
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

    // Distinct real-world-inspired capabilities must be authored per player;
    // no neutral team-wide rating should be treated as real scouting data.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Ratings", meta=(ClampMin="1", ClampMax="99"))
    int32 Vision = 65;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Ratings", meta=(ClampMin="1", ClampMax="99"))
    int32 LongPassing = 65;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Ratings", meta=(ClampMin="1", ClampMax="99"))
    int32 ShotPower = 65;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Ratings", meta=(ClampMin="1", ClampMax="99"))
    int32 DefensiveAwareness = 65;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Ratings", meta=(ClampMin="1", ClampMax="99"))
    int32 Composure = 65;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Ratings", meta=(ClampMin="1", ClampMax="99"))
    int32 Strength = 65;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sports Arcade|Ratings", meta=(ClampMin="1", ClampMax="99"))
    int32 WeakFootAbility = 65;

    /** Relative ability under fatigue and pressure; neutral at full fitness. */
    UFUNCTION(BlueprintPure, Category="Sports Arcade|Ratings")
    static float ContextualRatingFactor(int32 Rating, float FitnessNormalized, float PressureNormalized);

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Ratings")
    static float RatingFactor(int32 Rating);
};
