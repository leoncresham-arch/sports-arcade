#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SportsArcadePenaltyShootout.generated.h"

/** Offline penalty shootout state. Animations, controls and referee enforcement are separate. */
UCLASS(BlueprintType)
class SPORTSARCADE_API USportsArcadePenaltyShootout : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Penalties")
    int32 HomeAttempts = 0;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Penalties")
    int32 AwayAttempts = 0;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Penalties")
    int32 HomeScored = 0;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Penalties")
    int32 AwayScored = 0;

    /** -1 means undecided; 0 = home and 1 = away. */
    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Penalties")
    int32 WinningTeam = INDEX_NONE;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Penalties")
    void ResetShootout();

    /** Home takes the first kick; then strict alternating attempts. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Penalties")
    bool RecordAttempt(int32 TeamIndex, bool bScored);

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Penalties")
    int32 GetNextKickingTeam() const;

    /** Preferred next authored lineup slot, or INDEX_NONE when shootout is decided. */
    UFUNCTION(BlueprintPure, Category="Sports Arcade|Penalties")
    int32 GetNextKickerLineupSlot() const;
};
