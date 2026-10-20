#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SportsArcadeLeagueTable.h"
#include "SportsArcadeGameMode.generated.h"

class ASportsArcadeBall;
class ASportsArcadeCharacter;
class ASportsArcadeTeamRoster;
class USportsArcadeSeasonSave;
class USportsArcadeCupSave;
class USportsArcadePenaltyShootout;
class USportsArcadeFootballDatabase;

UENUM(BlueprintType)
enum class ESportsArcadeRestartType : uint8
{
    None, ThrowIn, CornerKick, GoalKick, Kickoff, Undetermined
};

UENUM(BlueprintType)
enum class ESportsArcadeMatchPhase : uint8
{
    Kickoff,
    Playing,
    Stoppage,
    HalfTime,
    FullTime,
    AwaitingExtraTime,
    ExtraTimeFirstHalf,
    ExtraTimeInterval,
    ExtraTimeSecondHalf,
    AwaitingPenalties,
    PenaltyShootout
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSportsArcadeGoalScoredEvent, int32, HomeScore, int32, AwayScore, bool, bOwnGoal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSportsArcadeMatchPhaseEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSportsArcadeBallOutEvent, FVector, ExitLocation, int32, LastTouchTeam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSportsArcadePenaltyReviewedEvent, int32, KickingTeam, bool, bScored);

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

    /** Fired after a previously undetermined restart receives a valid award. */
    UPROPERTY(BlueprintAssignable, Category="Sports Arcade|Match|Restart")
    FSportsArcadeMatchPhaseEvent OnRestartAwarded;

    /** Fired when the physics-backed cup penalty is actually struck. */
    UPROPERTY(BlueprintAssignable, Category="Sports Arcade|Cup|Penalties")
    FSportsArcadeMatchPhaseEvent OnPhysicalPenaltyKicked;

    UPROPERTY(BlueprintAssignable, Category="Sports Arcade|Cup|Penalties")
    FSportsArcadePenaltyReviewedEvent OnCupPenaltyReviewed;

    /** Heuristic goal/miss observation for review, not an official ruling. */
    UPROPERTY(BlueprintAssignable, Category="Sports Arcade|Cup|Penalties")
    FSportsArcadePenaltyReviewedEvent OnCupPenaltyOutcomeSuggested;

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

    /** Completes a staged manual set-piece using the awarded taker. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Match|Restart")
    bool TakeManualRestart(const FVector& KickDirection);

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Match|Restart")
    bool CanTakeManualRestart() const;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Match")
    float GetMatchProgress() const;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Match")
    ESportsArcadeMatchPhase GetCurrentMatchPhase() const;

    /** Football clock in match minutes, normalized for configurable period lengths (0–120). */
    UFUNCTION(BlueprintPure, Category="Sports Arcade|Match")
    float GetMatchClockMinutes() const;

    /** 1-2 in regulation, 3-4 in extra time; stays at final period after full-time. */
    UFUNCTION(BlueprintPure, Category="Sports Arcade|Match")
    int32 GetMatchPeriodNumber() const;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Match|Restart")
    bool CanResumePendingRestart() const;

    /** Referee decision for a boundary exit whose last touch was unknown. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Match|Restart")
    bool AwardUndeterminedRestart(int32 AwardedTeam);

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Match")
    void RecordGoal(bool bHomeTeamScored);

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Match|Stats")
    void RecordShotAttempt(int32 ShootingTeam);

    /** Select a scheduled league fixture before playing; scores are committed once at full time. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Season")
    bool BindSeasonFixture(USportsArcadeSeasonSave* Season, int32 FixtureIndex);

    /** Explicitly persist the bound season in the requested local slot. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Season")
    bool SaveBoundSeasonToSlot(const FString& SlotName, int32 UserIndex = 0) const;

    /** Returns false unless the latest match result was recorded in the bound season. */
    UFUNCTION(BlueprintPure, Category="Sports Arcade|Season")
    bool IsBoundSeasonResultRecorded() const;

    /** Read-only selection data for season menus and post-match screens. */
    UFUNCTION(BlueprintPure, Category="Sports Arcade|Season")
    USportsArcadeSeasonSave* GetBoundSeason() const;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Season")
    int32 GetBoundFixtureIndex() const;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Season")
    bool GetBoundSeasonStandings(TArray<FSportsArcadeLeagueStanding>& OutStandings) const;

    /** After full time, advance to the next fixture in the bound season. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Season")
    bool AdvanceToNextSeasonFixture();

    /** Starts the next knockout tie; drawn scores require external tie resolution. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup")
    bool BindNextCupFixture(USportsArcadeCupSave* Cup);

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup")
    bool IsCupResultRecorded() const;

    /** Drawn knockout games remain unresolved until extra time/penalties exist. */
    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup")
    bool IsCupTieBreakRequired() const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sports Arcade|Cup|Extra Time", meta=(ClampMin="1.0"))
    float ExtraTimeHalfDurationSeconds = 60.f;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Cup|Extra Time")
    float ExtraTimeElapsedSeconds = 0.f;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup|Extra Time")
    bool IsCupExtraTimeRequired() const;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup|Extra Time")
    bool IsCupExtraTimeActive() const;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup|Extra Time")
    float GetExtraTimeProgress() const;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup|Extra Time")
    bool StartCupExtraTime();

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup|Extra Time")
    bool ResumeCupExtraTimeSecondHalf();

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup|Penalties")
    bool StartCupPenaltyShootout();

    /** Optional physics-backed shootout workflow. Result still requires explicit referee judgment. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sports Arcade|Cup|Penalties")
    bool bRequirePhysicalPenaltyKicks = false;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup|Penalties")
    bool StageCupPenaltyKick();

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup|Penalties")
    bool CanStageCupPenaltyKick() const;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup|Penalties")
    bool KickStagedCupPenalty(const FVector& Direction);

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup|Penalties")
    bool CanKickStagedCupPenalty() const;

    /** Physical kick has been taken and is waiting for referee scoring review. */
    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup|Penalties")
    bool IsCupPenaltyAwaitingResult() const;

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup|Penalties")
    int32 GetCupPenaltyDefendingTeam() const;

    /** Last attributed physical penalty contact was by the defending goalkeeper. */
    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup|Penalties")
    bool WasCupPenaltyTouchedByKeeper() const;

    /** -1 pending, 0 suspected miss, 1 suspected goal; only a referee confirms result. */
    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup|Penalties")
    int32 GetSuggestedCupPenaltyOutcome() const;

    /** Seconds since the physical kick; zero when no attempt is awaiting review. */
    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup|Penalties")
    float GetCupPenaltyObservationElapsedSeconds() const;

    /** Remaining seconds until the referee-assist hard cap, not an automatic ruling. */
    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup|Penalties")
    float GetCupPenaltyObservationRemainingSeconds() const;

    /** Explicit referee confirmation of the nonauthoritative ball-flight suggestion. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup|Penalties")
    bool ConfirmSuggestedCupPenaltyOutcome();

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup|Penalties")
    bool CanConfirmSuggestedCupPenaltyOutcome() const;

    /** Offline referee review: record outcome only after a physical attempt. */
    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup|Penalties")
    bool ResolvePhysicalCupPenalty(bool bScored);

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup|Penalties")
    ASportsArcadeCharacter* GetStagedCupPenaltyTaker() const;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup|Penalties")
    bool RecordCupPenaltyAttempt(int32 KickingTeam, bool bScored);

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup|Penalties")
    USportsArcadePenaltyShootout* GetCupPenaltyShootout() const;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup")
    bool AdvanceBoundCupRound();

    UFUNCTION(BlueprintPure, Category="Sports Arcade|Cup")
    USportsArcadeCupSave* GetBoundCup() const;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Cup")
    bool SaveBoundCupToSlot(const FString& SlotName, int32 UserIndex = 0) const;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Season")
    FName SelectedHomeClubId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sports Arcade|Season")
    TObjectPtr<USportsArcadeFootballDatabase> FootballCatalog;

    UPROPERTY(BlueprintReadOnly, Category="Sports Arcade|Season")
    FName SelectedAwayClubId;

    UFUNCTION(BlueprintCallable, Category="Sports Arcade|Team")
    bool SwitchToNearestTeammate(int32 TeamIndex);

private:
    UPROPERTY(Transient)
    TObjectPtr<ASportsArcadeBall> MatchBall;
    FVector PreviousBallPosition = FVector::ZeroVector;
    bool bHasPreviousBallPosition = false;
    bool bSecondHalfStarted = false;
    bool bManualRestartStaged = false;
    UPROPERTY(Transient)
    TObjectPtr<USportsArcadeSeasonSave> BoundSeason;
    int32 BoundFixtureIndex = INDEX_NONE;
    bool bSeasonResultRecorded = false;
    UPROPERTY(Transient)
    TObjectPtr<USportsArcadeCupSave> BoundCup;
    UPROPERTY(Transient)
    TObjectPtr<USportsArcadePenaltyShootout> ActivePenaltyShootout;
    UPROPERTY(Transient)
    TObjectPtr<ASportsArcadeCharacter> StagedPenaltyTaker;
    bool bPenaltyKickStaged = false;
    FVector StagedPenaltySpot = FVector::ZeroVector;
    bool bPenaltyKickTaken = false;
    bool bHasPreviousPenaltyBallPosition = false;
    FVector PreviousPenaltyBallPosition = FVector::ZeroVector;
    int32 SuggestedCupPenaltyOutcome = INDEX_NONE;
    float PhysicalPenaltyObservationSeconds = 0.f;
    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Cup|Penalties", meta=(ClampMin="1.0"))
    float PhysicalPenaltyObservationTimeoutSeconds = 8.f;
    /** Safety cap for a moving or bouncing attempt that never reaches a boundary. */
    UPROPERTY(EditDefaultsOnly, Category="Sports Arcade|Cup|Penalties", meta=(ClampMin="1.0"))
    float PhysicalPenaltyMaximumObservationSeconds = 20.f;
    void ObservePhysicalCupPenalty();
    bool bCupResultRecorded = false;
    bool bCupExtraTimePlayed = false;
    bool bExtraTimeActive = false;
    bool bExtraTimeSecondHalf = false;
    int32 BoundCupFixtureIndex = INDEX_NONE;
    void CommitCupResult();
    void CommitSeasonResult();
    void ClearPendingRestart();
    void StageKickoff(int32 TakingTeam);
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
