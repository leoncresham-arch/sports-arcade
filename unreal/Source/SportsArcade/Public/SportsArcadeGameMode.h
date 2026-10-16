#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SportsArcadeGameMode.generated.h"

class ASportsArcadeBall;
class ASportsArcadeCharacter;
class ASportsArcadeTeamRoster;

UENUM(BlueprintType)
enum class ESportsArcadeRestartType : uint8
{
    None, ThrowIn, CornerKick, GoalKick, Undetermined
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSportsArcadeGoalScoredEvent, int32, HomeScore, int32, AwayScore, bool, bOwnGoal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSportsArcadeMatchPhaseEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSportsArcadeBallOutEvent, FVector, ExitLocation, int32, LastTouchTeam);

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sports Arcade|Pitch", meta=(ClampMin="2000.0"))
    float PitchHalfLengthCm = 5250.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sports Arcade|Pitch", meta=(ClampMin="1000.0"))
    float PitchHalfWidthCm = 3400.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sports Arcade|Pitch", meta=(ClampMin="100.0"))
    float GoalHalfWidthCm = 366.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sports Arcade|Pitch", meta=(ClampMin="100.0"))
    float CrossbarHeightCm = 244.f;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    float ElapsedMatchSeconds = 0.f;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    int32 HomeGoals = 0;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    int32 AwayGoals = 0;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match|Stats")
    int32 HomeShots = 0;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match|Stats")
    int32 AwayShots = 0;

    UPROPERTY(BlueprintAssignable, Category="Sports Arcade|Match")
    FSportsArcadeGoalScoredEvent OnGoalScored;

    UPROPERTY(BlueprintAssignable, Category="Sports Arcade|Match")
    FSportsArcadeBallOutEvent OnBallOutOfPlay;

    /** Prototype stoppage: requires a referee restart implementation. */
    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    bool bBallOutOfPlay = false;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    ESportsArcadeRestartType PendingRestartType = ESportsArcadeRestartType::None;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    int32 PendingRestartTeam = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    FVector PendingRestartLocation = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    TObjectPtr<ASportsArcadeCharacter> PendingRestartTaker;

    UPROPERTY(BlueprintAssignable, Category="Sports Arcade|Match")
    FSportsArcadeMatchPhaseEvent OnHalfTime;

    UPROPERTY(BlueprintAssignable, Category="Sports Arcade|Match")
    FSportsArcadeMatchPhaseEvent OnFullTime;

    UPROPERTY(BlueprintAssignable, Category="Sports Arcade|Match")
    FSportsArcadeMatchPhaseEvent OnMatchReset;

    UPROPERTY(BlueprintAssignable, Category="Sports Arcade|Match")
    FSportsArcadeMatchPhaseEvent OnPlayResumed;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    TObjectPtr<ASportsArcadeCharacter> LastGoalTouchPlayer;

    /** Simplified attribution: a known player last touched the ball for the opposing scoring side. */
    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    bool bLastGoalWasOwnGoal = false;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    bool bMatchFinished = false;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Match")
    bool bHalfTime = false;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Match")
    void ResumeSecondHalf();

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Match")
    void ResetMatch();

    /** Temporary offline restart from centre; not a throw-in/corner/goal-kick. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Match")
    void ResumeAfterBallOut();

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Match")
    float GetMatchProgress() const;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Match")
    void RecordGoal(bool bHomeTeamScored);

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Match|Stats")
    void RecordShotAttempt(int32 ShootingTeam);

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Team")
    bool SwitchToNearestTeammate(int32 TeamIndex);

private:
    UPROPERTY(Transient)
    TObjectPtr<ASportsArcadeBall> MatchBall;
    FVector PreviousBallPosition = FVector::ZeroVector;
    bool bHasPreviousBallPosition = false;
    bool bSecondHalfStarted = false;
    void ClearPendingRestart();
    ASportsArcadeCharacter* FindRestartTaker(int32 Team, const FVector& Location) const;

    /** Use the placed TV camera in a visual review level when one exists. */
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Visual Review")
    bool bUseBroadcastCameraIfPresent = true;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Visual Review")
    bool bTrackReferenceFootball = true;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Team")
    bool bAutoPossessHomeRosterStarter = true;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Match")
    bool bAutoKickProvisionalRestart = true;

    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Match", meta=(ClampMin="100.0"))
    float ProvisionalRestartImpulse = 1500.f;
};
