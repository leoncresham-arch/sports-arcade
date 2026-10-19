#include "SportsArcadeGameMode.h"
#include "SportsArcadeCharacter.h"
#include "SportsArcadeBroadcastCamera.h"
#include "SportsArcadeBall.h"
#include "SportsArcadeTeamRoster.h"
#include "SportsArcadeSeasonSave.h"
#include "SportsArcadeFootballDatabase.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

ASportsArcadeGameMode::ASportsArcadeGameMode()
{
    DefaultPawnClass = ASportsArcadeCharacter::StaticClass();
    PrimaryActorTick.bCanEverTick = true;
}

void ASportsArcadeGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (GetWorld())
    {
        for (TActorIterator<ASportsArcadeTeamRoster> RosterIt(GetWorld()); RosterIt; ++RosterIt)
        {
            if (!RosterIt->HasCompleteLineup())
            {
                UE_LOG(LogTemp, Warning, TEXT("SportsArcade: team %d has an incomplete or invalid lineup"), RosterIt->TeamIndex);
            }
        }
    }

    if (bAutoPossessHomeRosterStarter && GetWorld())
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        for (TActorIterator<ASportsArcadeTeamRoster> It(GetWorld()); PC && It; ++It)
        {
            if (It->TeamIndex != 0) continue;
            ASportsArcadeCharacter* Starter = nullptr;
            for (ASportsArcadeCharacter* Candidate : It->Players)
            {
                if (IsValid(Candidate) && Candidate->TeamIndex == 0 && Candidate->LineupSlot == 9)
                {
                    Starter = Candidate;
                    break;
                }
            }
            if (IsValid(Starter) && Starter->TeamIndex == 0)
            {
                PC->Possess(Starter);
            }
            break;
        }
    }

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
    if (bMatchFinished || bHalfTime || bBallOutOfPlay || DeltaSeconds <= 0.f || !GetWorld()) return;

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
            const float Threshold = PitchHalfLengthCm + MatchBall->BallRadiusCm;
            // Interpolate at the actual crossed X plane; endpoint alone can miss a fast diagonal shot.
            const FVector Travel = Position - PreviousBallPosition;
            const auto CrossesGoal = [&](float PlaneX) -> bool
            {
                if (FMath::Abs(Travel.X) < KINDA_SMALL_NUMBER) return false;
                const float T = (PlaneX - PreviousBallPosition.X) / Travel.X;
                if (T < 0.f || T > 1.f) return false;
                const FVector Crossing = PreviousBallPosition + Travel * T;
                // A touchline exit earlier in this frame invalidates a later goal-plane crossing.
                const float TouchEdge = PitchHalfWidthCm + MatchBall->BallRadiusCm;
                if (FMath::Abs(PreviousBallPosition.Y) > TouchEdge) return false;
                if (FMath::Abs(Travel.Y) > KINDA_SMALL_NUMBER)
                {
                    const float TouchPlane = Travel.Y > 0.f ? TouchEdge : -TouchEdge;
                    const float TouchT = (TouchPlane - PreviousBallPosition.Y) / Travel.Y;
                    if (TouchT >= 0.f && TouchT < T) return false;
                }
                return FMath::Abs(Crossing.Y) + MatchBall->BallRadiusCm < GoalHalfWidthCm &&
                    Crossing.Z >= MatchBall->BallRadiusCm && Crossing.Z + MatchBall->BallRadiusCm < CrossbarHeightCm;
            };
            if (CrossesGoal(Threshold) && PreviousBallPosition.X < Threshold && Position.X >= Threshold)
            {
                LastGoalTouchPlayer = MatchBall->GetLastTouchPlayer();
                bLastGoalWasOwnGoal = IsValid(LastGoalTouchPlayer) && LastGoalTouchPlayer->TeamIndex == 1;
                RecordGoal(true);
                StageKickoff(1);
                return;
            }
            else if (CrossesGoal(-Threshold) && PreviousBallPosition.X > -Threshold && Position.X <= -Threshold)
            {
                LastGoalTouchPlayer = MatchBall->GetLastTouchPlayer();
                bLastGoalWasOwnGoal = IsValid(LastGoalTouchPlayer) && LastGoalTouchPlayer->TeamIndex == 0;
                RecordGoal(false);
                StageKickoff(0);
                return;
            }
        }
        // Full-ball boundary crossing, classified by last attributed team.
        // Compare segment crossing fractions so a corner exit is assigned to
        // the first crossed boundary rather than the furthest endpoint.
        const float EdgeX = PitchHalfLengthCm + MatchBall->BallRadiusCm;
        const float EdgeY = PitchHalfWidthCm + MatchBall->BallRadiusCm;
        if (FMath::Abs(Position.Y) > EdgeY || FMath::Abs(Position.X) > EdgeX)
        {
            const FVector Travel = Position - PreviousBallPosition;
            float Tx = 2.f;
            float Ty = 2.f;
            if (bHasPreviousBallPosition)
            {
                if (FMath::Abs(Travel.X) > KINDA_SMALL_NUMBER)
                {
                    const float Line = Position.X >= 0.f ? EdgeX : -EdgeX;
                    Tx = (Line - PreviousBallPosition.X) / Travel.X;
                    if (Tx < 0.f || Tx > 1.f) Tx = 2.f;
                }
                if (FMath::Abs(Travel.Y) > KINDA_SMALL_NUMBER)
                {
                    const float Line = Position.Y >= 0.f ? EdgeY : -EdgeY;
                    Ty = (Line - PreviousBallPosition.Y) / Travel.Y;
                    if (Ty < 0.f || Ty > 1.f) Ty = 2.f;
                }
            }
            const bool bTouchline = Ty < Tx || (Tx >= 2.f && FMath::Abs(Position.Y) > EdgeY);
            const int32 LastTeam = MatchBall->GetLastTouchTeamIndex();
            PendingRestartTeam = (LastTeam == 0 || LastTeam == 1) ? 1 - LastTeam : INDEX_NONE;
            const float SideX = Position.X >= 0.f ? 1.f : -1.f;
            const float SideY = Position.Y >= 0.f ? 1.f : -1.f;
            const float RestZ = MatchBall->BallRadiusCm + 2.f;
            if (bTouchline)
            {
                PendingRestartType = PendingRestartTeam == INDEX_NONE
                    ? ESportsArcadeRestartType::Undetermined : ESportsArcadeRestartType::ThrowIn;
                const float CrossY = SideY * EdgeY;
                const float Alpha = FMath::Abs(Travel.Y) > KINDA_SMALL_NUMBER
                    ? FMath::Clamp((CrossY - PreviousBallPosition.Y) / Travel.Y, 0.f, 1.f) : 1.f;
                const float CrossX = FMath::Clamp(PreviousBallPosition.X + Travel.X * Alpha, -PitchHalfLengthCm + 100.f, PitchHalfLengthCm - 100.f);
                PendingRestartLocation = FVector(CrossX, SideY * (PitchHalfWidthCm - 40.f), RestZ);
            }
            else
            {
                const int32 DefendingTeam = SideX > 0.f ? 1 : 0;
                if (PendingRestartTeam == INDEX_NONE)
                {
                    PendingRestartType = ESportsArcadeRestartType::Undetermined;
                    PendingRestartLocation = FVector(SideX * (PitchHalfLengthCm - 550.f), 0.f, RestZ);
                }
                else if (LastTeam == DefendingTeam)
                {
                    PendingRestartType = ESportsArcadeRestartType::CornerKick;
                    PendingRestartTeam = 1 - DefendingTeam;
                    PendingRestartLocation = FVector(SideX * (PitchHalfLengthCm - 60.f), SideY * (PitchHalfWidthCm - 60.f), RestZ);
                }
                else
                {
                    PendingRestartType = ESportsArcadeRestartType::GoalKick;
                    PendingRestartTeam = DefendingTeam;
                    PendingRestartLocation = FVector(SideX * (PitchHalfLengthCm - 500.f), SideY * 500.f, RestZ);
                }
            }
            PendingRestartTaker = FindRestartTaker(PendingRestartTeam, PendingRestartLocation);
            // Prefer the team's actual goalkeeper for an uncontested goal kick.
            if (PendingRestartType == ESportsArcadeRestartType::GoalKick)
            {
                for (TActorIterator<ASportsArcadeTeamRoster> Roster(GetWorld()); Roster; ++Roster)
                {
                    if (Roster->TeamIndex != PendingRestartTeam) continue;
                    for (ASportsArcadeCharacter* Candidate : Roster->Players)
                    {
                        if (IsValid(Candidate) && Candidate->TeamIndex == PendingRestartTeam &&
                            Candidate->LineupSlot == 0)
                        {
                            PendingRestartTaker = Candidate;
                            break;
                        }
                    }
                    break;
                }
            }
            bBallOutOfPlay = true;
            MatchBall->StopMotion();
            bHasPreviousBallPosition = false;
            OnBallOutOfPlay.Broadcast(Position, LastTeam);
            return;
        }
        PreviousBallPosition = MatchBall->GetActorLocation();
        bHasPreviousBallPosition = true;
    }
    ElapsedMatchSeconds = FMath::Min(ElapsedMatchSeconds + DeltaSeconds, 2.f * HalfDurationSeconds);
    if (!bSecondHalfStarted && ElapsedMatchSeconds >= HalfDurationSeconds)
    {
        ElapsedMatchSeconds = HalfDurationSeconds;
        bHalfTime = true;
        // Prevent a loose ball from scoring during the intermission.
        if (IsValid(MatchBall))
        {
            MatchBall->ResetToCentre();
            bHasPreviousBallPosition = false;
        }
        OnHalfTime.Broadcast();
    }
    if (ElapsedMatchSeconds >= 2.f * HalfDurationSeconds && !bMatchFinished)
    {
        bMatchFinished = true;
        // A stationary ball keeps the final score presentation deterministic.
        if (IsValid(MatchBall))
        {
            MatchBall->StopMotion();
            bHasPreviousBallPosition = false;
        }
        CommitSeasonResult();
        OnFullTime.Broadcast();
    }
}

void ASportsArcadeGameMode::RecordGoal(bool bHomeTeamScored)
{
    if (bMatchFinished || bHalfTime || bBallOutOfPlay) return;
    if (bHomeTeamScored) ++HomeGoals;
    else ++AwayGoals;
    OnGoalScored.Broadcast(HomeGoals, AwayGoals, bLastGoalWasOwnGoal);
}

void ASportsArcadeGameMode::RecordShotAttempt(int32 ShootingTeam)
{
    if (bMatchFinished || bHalfTime || bBallOutOfPlay) return;
    if (ShootingTeam == 0) ++HomeShots;
    else if (ShootingTeam == 1) ++AwayShots;
}

void ASportsArcadeGameMode::ResumeSecondHalf()
{
    if (!bHalfTime || bMatchFinished) return;
    bHalfTime = false;
    bBallOutOfPlay = false;
    ClearPendingRestart();
    bSecondHalfStarted = true;
    bHasPreviousBallPosition = false;
    StageKickoff(1);
    // Resume event fires only once the second-half kickoff is taken.
}

void ASportsArcadeGameMode::ResetMatch()
{
    ElapsedMatchSeconds = 0.f;
    HomeGoals = 0;
    AwayGoals = 0;
    HomeShots = 0;
    AwayShots = 0;
    LastGoalTouchPlayer = nullptr;
    bLastGoalWasOwnGoal = false;
    bHalfTime = false;
    bBallOutOfPlay = false;
    ClearPendingRestart();
    bSecondHalfStarted = false;
    bMatchFinished = false;
    bHasPreviousBallPosition = false;
    if (IsValid(MatchBall)) MatchBall->ResetToCentre();
    if (GetWorld())
    {
        for (TActorIterator<ASportsArcadeCharacter> It(GetWorld()); It; ++It)
        {
            It->ResetToKickoffPosition();
        }
    }
    bSeasonResultRecorded = false;
    OnMatchReset.Broadcast();
}

void ASportsArcadeGameMode::ResumeAfterBallOut()
{
    if (!bBallOutOfPlay || bHalfTime || bMatchFinished) return;
    if (PendingRestartType == ESportsArcadeRestartType::Undetermined ||
        PendingRestartType == ESportsArcadeRestartType::None ||
        (PendingRestartTeam != 0 && PendingRestartTeam != 1) || !IsValid(MatchBall) ||
        !IsValid(PendingRestartTaker) || PendingRestartTaker->TeamIndex != PendingRestartTeam) return;
    // Provisional in-field set-piece setup; official throw mechanics remain TODO.
    const float InwardX = PendingRestartLocation.X >= 0.f ? -1.f : 1.f;
    const float InwardY = PendingRestartLocation.Y >= 0.f ? -1.f : 1.f;
    const FVector TakerPosition(PendingRestartLocation.X + InwardX * 85.f,
        PendingRestartLocation.Y + InwardY * 55.f,
        PendingRestartTaker->GetActorLocation().Z);
    if (!PendingRestartTaker->SetActorLocation(TakerPosition, false, nullptr, ETeleportType::TeleportPhysics))
    {
        return;
    }
    PendingRestartTaker->GetCharacterMovement()->StopMovementImmediately();
    MatchBall->PlaceAtRest(PendingRestartLocation);
    if (bAutoKickProvisionalRestart)
    {
        // Offline only: approximates putting the ball back into play.
        // A real throw-in needs hands, animations and set-piece positioning.
        const FVector Direction = PendingRestartType == ESportsArcadeRestartType::Kickoff
            ? FVector(PendingRestartTeam == 0 ? 1.f : -1.f, 0.f, 0.08f)
            : PendingRestartType == ESportsArcadeRestartType::ThrowIn
                ? FVector(0.f, -FMath::Sign(PendingRestartLocation.Y), 0.25f)
                : FVector(PendingRestartTeam == 0 ? 1.f : -1.f, -PendingRestartLocation.Y / PitchHalfWidthCm, 0.22f);
        if (!MatchBall->KickByPlayer(PendingRestartTaker, Direction, ProvisionalRestartImpulse, 210.f))
        {
            return;
        }
    }
    bBallOutOfPlay = false;
    bHasPreviousBallPosition = false;
    ClearPendingRestart();
    OnPlayResumed.Broadcast();
}

void ASportsArcadeGameMode::StageKickoff(int32 TakingTeam)
{
    if (TakingTeam != 0 && TakingTeam != 1) return;
    if (!IsValid(MatchBall) && GetWorld())
    {
        for (TActorIterator<ASportsArcadeBall> It(GetWorld()); It; ++It)
        {
            MatchBall = *It;
            break;
        }
    }
    if (IsValid(MatchBall)) MatchBall->ResetToCentre();
    PendingRestartType = ESportsArcadeRestartType::Kickoff;
    PendingRestartTeam = TakingTeam;
    PendingRestartLocation = FVector(0.f, 0.f, IsValid(MatchBall) ? MatchBall->BallRadiusCm + 2.f : 14.f);
    // Reset all on-field footballers to their authored kickoff layout.
    // The restart taker is selected after repositioning, never from a stale
    // post-goal chase location. Keep authored formations intact.
    if (GetWorld())
    {
        for (TActorIterator<ASportsArcadeCharacter> It(GetWorld()); It; ++It)
        {
            It->ResetToKickoffPosition();
        }
    }
    PendingRestartTaker = FindRestartTaker(TakingTeam, PendingRestartLocation);
    // Prefer an assigned striker over the nearest midfielder or goalkeeper.
    if (GetWorld())
    {
        for (TActorIterator<ASportsArcadeTeamRoster> Roster(GetWorld()); Roster; ++Roster)
        {
            if (Roster->TeamIndex != TakingTeam) continue;
            for (ASportsArcadeCharacter* Candidate : Roster->Players)
            {
                if (IsValid(Candidate) && Candidate->TeamIndex == TakingTeam && Candidate->LineupSlot == 9)
                {
                    PendingRestartTaker = Candidate;
                    break;
                }
            }
            break;
        }
    }
    bBallOutOfPlay = true;
    bHasPreviousBallPosition = false;
}

void ASportsArcadeGameMode::ClearPendingRestart()
{
    PendingRestartType = ESportsArcadeRestartType::None;
    PendingRestartTeam = INDEX_NONE;
    PendingRestartLocation = FVector::ZeroVector;
    PendingRestartTaker = nullptr;
}

ASportsArcadeCharacter* ASportsArcadeGameMode::FindRestartTaker(int32 Team, const FVector& Location) const
{
    if (!GetWorld() || (Team != 0 && Team != 1)) return nullptr;
    ASportsArcadeCharacter* Best = nullptr;
    float BestDistance = TNumericLimits<float>::Max();
    for (TActorIterator<ASportsArcadeTeamRoster> It(GetWorld()); It; ++It)
    {
        if (It->TeamIndex != Team) continue;
        for (ASportsArcadeCharacter* Candidate : It->Players)
        {
            if (!IsValid(Candidate) || Candidate->TeamIndex != Team) continue;
            const float Distance = FVector::DistSquared2D(Candidate->GetActorLocation(), Location);
            if (Distance < BestDistance)
            {
                BestDistance = Distance;
                Best = Candidate;
            }
        }
    }
    if (!IsValid(Best))
    {
        // Single-footballer prototype maps may have no authored team roster.
        for (TActorIterator<ASportsArcadeCharacter> It(GetWorld()); It; ++It)
        {
            if (It->TeamIndex != Team) continue;
            const float Distance = FVector::DistSquared2D(It->GetActorLocation(), Location);
            if (Distance < BestDistance)
            {
                BestDistance = Distance;
                Best = *It;
            }
        }
    }
    return Best;
}

float ASportsArcadeGameMode::GetMatchProgress() const
{
    return FMath::Clamp(ElapsedMatchSeconds / FMath::Max(2.f * HalfDurationSeconds, 1.f), 0.f, 1.f);
}

bool ASportsArcadeGameMode::SwitchToNearestTeammate(int32 TeamIndex)
{
    if (!GetWorld() || TeamIndex < 0 || TeamIndex > 1 || bMatchFinished || bHalfTime || bBallOutOfPlay) return false;
    APlayerController* Controller = GetWorld()->GetFirstPlayerController();
    if (!IsValid(Controller)) return false;

    const FVector Target = IsValid(MatchBall)
        ? MatchBall->GetActorLocation()
        : Controller->GetPawn() ? Controller->GetPawn()->GetActorLocation() : FVector::ZeroVector;

    ASportsArcadeCharacter* BestPlayer = nullptr;
    float BestDistance = TNumericLimits<float>::Max();
    for (TActorIterator<ASportsArcadeTeamRoster> It(GetWorld()); It; ++It)
    {
        if (It->TeamIndex != TeamIndex) continue;
        for (ASportsArcadeCharacter* Candidate : It->Players)
        {
            if (!IsValid(Candidate) || Candidate == Controller->GetPawn() || Candidate->TeamIndex != TeamIndex) continue;
            const float Distance = FVector::DistSquared2D(Candidate->GetActorLocation(), Target);
            if (Distance < BestDistance)
            {
                BestPlayer = Candidate;
                BestDistance = Distance;
            }
        }
    }
    if (!IsValid(BestPlayer)) return false;
    APawn* PreviouslyControlled = Controller->GetPawn();
    Controller->Possess(BestPlayer);
    if (Controller->GetPawn() != BestPlayer) return false;
    if (IsValid(PreviouslyControlled) && PreviouslyControlled != BestPlayer && !PreviouslyControlled->GetController())
    {
        // Return the former user-controlled footballer to formation AI.
        PreviouslyControlled->SpawnDefaultController();
    }
    return true;
}

bool ASportsArcadeGameMode::BindSeasonFixture(USportsArcadeSeasonSave* Season, int32 FixtureIndex)
{
    if (!IsValid(Season) || !Season->Fixtures.IsValidIndex(FixtureIndex) ||
        Season->CompletedFixtureIndices.Contains(FixtureIndex))
        return false;

    const FSportsArcadeLeagueFixture& Fixture = Season->Fixtures[FixtureIndex];
    if (Fixture.HomeClubId.IsNone() || Fixture.AwayClubId.IsNone() ||
        Fixture.HomeClubId == Fixture.AwayClubId ||
        !Season->ClubIds.Contains(Fixture.HomeClubId) ||
        !Season->ClubIds.Contains(Fixture.AwayClubId))
        return false;

    if (IsValid(FootballCatalog))
    {
        FSportsArcadeClubRecord HomeClub;
        FSportsArcadeClubRecord AwayClub;
        if (!FootballCatalog->FindClubById(Fixture.HomeClubId, HomeClub) ||
            !FootballCatalog->FindClubById(Fixture.AwayClubId, AwayClub) ||
            HomeClub.Category != AwayClub.Category)
            return false;
    }
    // Season matches use the existing authored 11v11 teams, not shadow rosters.
    // Validate both before modifying either actor or the bound fixture.
    ASportsArcadeTeamRoster* HomeRoster = nullptr;
    ASportsArcadeTeamRoster* AwayRoster = nullptr;
    if (!GetWorld()) return false;
    for (TActorIterator<ASportsArcadeTeamRoster> It(GetWorld()); It; ++It)
    {
        if (It->TeamIndex == 0)
        {
            if (HomeRoster) return false;
            HomeRoster = *It;
        }
        else if (It->TeamIndex == 1)
        {
            if (AwayRoster) return false;
            AwayRoster = *It;
        }
    }
    if (!IsValid(HomeRoster) || !IsValid(AwayRoster) ||
        !HomeRoster->HasCompleteLineup() || !AwayRoster->HasCompleteLineup())
        return false;
    // Do not replace a match in progress or a result already recorded.
    if (ElapsedMatchSeconds > 0.f && !bMatchFinished) return false;
    // Bind club identities only after all match prerequisites pass.
    // An authored catalog can optionally assign stable player identities;
    // incomplete squad data must not prevent the prototype from running.
    HomeRoster->ClubId = Fixture.HomeClubId;
    AwayRoster->ClubId = Fixture.AwayClubId;
    if (IsValid(FootballCatalog))
    {
        if (!HomeRoster->AssignFootballerRecords(FootballCatalog) ||
            !AwayRoster->AssignFootballerRecords(FootballCatalog))
        {
            UE_LOG(LogTemp, Warning, TEXT("SportsArcade: fixture selected but a full catalog player mapping is not yet available"));
        }
    }
    BoundSeason = Season;
    BoundFixtureIndex = FixtureIndex;
    SelectedHomeClubId = Fixture.HomeClubId;
    SelectedAwayClubId = Fixture.AwayClubId;
    ResetMatch();
    return true;
}

void ASportsArcadeGameMode::CommitSeasonResult()
{
    if (bSeasonResultRecorded || !IsValid(BoundSeason) ||
        !BoundSeason->Fixtures.IsValidIndex(BoundFixtureIndex))
        return;
    bSeasonResultRecorded = BoundSeason->RecordFixtureResult(
        BoundFixtureIndex, HomeGoals, AwayGoals);
    if (!bSeasonResultRecorded)
    {
        UE_LOG(LogTemp, Warning, TEXT("SportsArcade: league result could not be recorded for fixture %d"), BoundFixtureIndex);
    }
}
