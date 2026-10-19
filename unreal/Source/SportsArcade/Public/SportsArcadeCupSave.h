#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SportsArcadeCupFixtures.h"
#include "SportsArcadeCupSave.generated.h"

/** Native cup progression state with explicit winners; tie resolution belongs to the referee. */
UCLASS(BlueprintType)
class SPORTSARCADE_API USportsArcadeCupSave : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Cup")
    FName CompetitionId;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Cup")
    TArray<FSportsArcadeCupFixture> CurrentRoundFixtures;

    /** Winners are recorded in current fixture order. */
    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Cup")
    TArray<FName> CurrentRoundWinners;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Cup")
    FName ChampionId;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup")
    bool StartCup(FName InCompetitionId, const TArray<FName>& ClubIds);

    /** Winner must belong to the next undecided fixture. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup")
    bool RecordWinner(FName WinningClubId);

    /** Progresses to the next round after all ties have a winner. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup")
    bool AdvanceRound();

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup")
    bool IsCupComplete() const;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup|Save")
    bool SaveCupToSlot(const FString& SlotName, int32 UserIndex = 0) const;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup|Save", meta=(WorldContext="WorldContextObject"))
    static USportsArcadeCupSave* LoadCupFromSlot(const UObject* WorldContextObject,
        const FString& SlotName, int32 UserIndex = 0);
};
