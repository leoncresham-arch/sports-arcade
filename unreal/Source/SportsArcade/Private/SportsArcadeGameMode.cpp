#include "SportsArcadeGameMode.h"
#include "SportsArcadeCharacter.h"
#include "SportsArcadeBroadcastCamera.h"
#include "SportsArcadeBall.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"

ASportsArcadeGameMode::ASportsArcadeGameMode()
{
    DefaultPawnClass = ASportsArcadeCharacter::StaticClass();
    PrimaryActorTick.bCanEverTick = true;
}

void ASportsArcadeGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (!bUseBroadcastCameraIfPresent || !GetWorld())
    {
        return;
    }

    ASportsArcadeBroadcastCamera* ReviewCamera = nullptr;
    for (TActorIterator<ASportsArcadeBroadcastCamera> It(GetWorld()); It; ++It)
    {
        ReviewCamera = *It;
        break;
    }

    // Preserve the player's ordinary camera in maps with no visual-review actor.
    if (!IsValid(ReviewCamera))
    {
        return;
    }

    if (bTrackReferenceFootball)
    {
        for (TActorIterator<ASportsArcadeBall> It(GetWorld()); It; ++It)
        {
            ReviewCamera->SetFocusActor(*It);
            break;
        }
    }

    if (APlayerController* Controller = GetWorld()->GetFirstPlayerController())
    {
        Controller->SetViewTargetWithBlend(ReviewCamera, 0.0f);
    }
}

void ASportsArcadeGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (bMatchFinished || bHalfTime || DeltaSeconds <= 0.f) return;
    ElapsedMatchSeconds = FMath::Min(ElapsedMatchSeconds + DeltaSeconds, 2.f * HalfDurationSeconds);

    if (!IsValid(MatchBall))
    {
        MatchBall = nullptr;
        bHasPreviousBallPosition = false;
        for (TActorIterator<ASportsArcadeBall> It(GetWorld()); It; ++It)
        {
            MatchBall = *It;
            break;
        }
    }
    if (IsValid(MatchBall))
    {
        const FVector Position = MatchBall->GetActorLocation();
        if (bHasPreviousBallPosition)
        {
            // Entire football must cross a 105 m pitch goal line between posts and below bar.
            const float Threshold = 5250.f + MatchBall->BallRadiusCm;
            const bool bInsideGoalMouth = FMath::Abs(Position.Y) < 366.f - MatchBall->BallRadiusCm &&
                Position.Z > MatchBall->BallRadiusCm && Position.Z < 244.f - MatchBall->BallRadiusCm;
            if (bInsideGoalMouth && PreviousBallPosition.X < Threshold && Position.X >= Threshold)
            {
                RecordGoal(true);
                MatchBall->ResetToCentre();
                bHasPreviousBallPosition = false;
            }
            else if (bInsideGoalMouth && PreviousBallPosition.X > -Threshold && Position.X <= -Threshold)
            {
                RecordGoal(false);
                MatchBall->ResetToCentre();
                bHasPreviousBallPosition = false;
            }
        }
        if (bHasPreviousBallPosition || (FMath::Abs(Position.X) < 5250.f))
        {
            PreviousBallPosition = Position;
            bHasPreviousBallPosition = true;
        }
    }
    if (!bSecondHalfStarted && ElapsedMatchSeconds >= HalfDurationSeconds)
    {
        ElapsedMatchSeconds = HalfDurationSeconds;
        bHalfTime = true;
    }
    if (ElapsedMatchSeconds >= 2.f * HalfDurationSeconds) bMatchFinished = true;
}

void ASportsArcadeGameMode::RecordGoal(bool bHomeTeamScored)
{
    if (bMatchFinished) return;
    if (bHomeTeamScored) ++HomeGoals;
    else ++AwayGoals;
}

void ASportsArcadeGameMode::ResumeSecondHalf()
{
    if (!bHalfTime || bMatchFinished) return;
    bHalfTime = false;
    bSecondHalfStarted = true;
    bHasPreviousBallPosition = false;
    if (IsValid(MatchBall)) MatchBall->ResetToCentre();
}
