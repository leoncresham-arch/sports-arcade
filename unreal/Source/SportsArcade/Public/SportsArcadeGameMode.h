#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SportsArcadeGameMode.generated.h"

class ASportsArcadeBall;
class ASportsArcadeTeamRoster;

/** PC-first technical foundation. Match rules will be implemented after art vertical slice. */
UCLASS()
class SPORTSARCADE_API ASportsArcadeGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ASportsArcadeGameMode();
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sports Arcade|Match", meta=(ClampMin="1.0"))
    float HalfDurationSeconds = 300.f;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    float ElapsedMatchSeconds = 0.f;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    int32 HomeGoals = 0;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    int32 AwayGoals = 0;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    bool bMatchFinished = false;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    bool bHalfTime = false;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Match")
    void ResumeSecondHalf();

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Match")
    void ResetMatch();

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Match")
    float GetMatchProgress() const;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Match")
    void RecordGoal(bool bHomeTeamScored);

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Team")
    bool SwitchToNearestTeammate(int32 TeamIndex);

private:
    UPROPERTY(Transient)
    TObjectPtr<ASportsArcadeBall> MatchBall;
    FVector PreviousBallPosition = FVector::ZeroVector;
    bool bHasPreviousBallPosition = false;
    bool bSecondHalfStarted = false;

    /** Use the placed TV camera in a visual review level when one exists. */
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Visual Review")
    bool bUseBroadcastCameraIfPresent = true;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Visual Review")
    bool bTrackReferenceFootball = true;
};
