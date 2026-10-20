#include "SportsArcadeTeamAIController.h"
#include "SportsArcadeTeamRoster.h"
#include "SportsArcadeCharacter.h"
#include "SportsArcadeBall.h"
#include "SportsArcadeGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Engine/World.h"

ASportsArcadeTeamAIController::ASportsArcadeTeamAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASportsArcadeTeamAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!FMath::IsFinite(DeltaSeconds) || DeltaSeconds <= 0.f) return;
    DecisionCountdown -= DeltaSeconds;
    ActionCooldownRemaining = FMath::Max(0.f, ActionCooldownRemaining - DeltaSeconds);
    if (!GetWorld()) return;
    if (DecisionCountdown > 0.f) return;
    DecisionCountdown = FMath::Max(DecisionIntervalSeconds, 0.05f);

    ASportsArcadeGameMode* Match = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this));
    // Only the defending goalkeeper may react during a completed cup match's
    // physical penalty attempt. All outfield AI remain stopped.
    if (IsValid(Match) && Match->IsCupPenaltyAwaitingResult())
    {
        ASportsArcadeCharacter* Keeper = Cast<ASportsArcadeCharacter>(GetPawn());
        if (!IsValid(Keeper) || Keeper->LineupSlot != 0 ||
            Keeper->TeamIndex != Match->GetCupPenaltyDefendingTeam())
        {
            StopMovement();
            return;
        }
        for (TActorIterator<ASportsArcadeBall> Ball(GetWorld()); Ball; ++Ball)
        {
            if (!IsValid(*Ball)) continue;
            FVector Target = Keeper->GetActorLocation();
            // Project an incoming shot toward the keeper plane rather than only
            // following its current lateral position. Keep this movement bounded.
            const FVector BallPosition = Ball->GetActorLocation();
            const FVector BallVelocity = Ball->GetVelocity();
            const float KeeperPlaneX = Keeper->GetActorLocation().X;
            const float TravelSeconds = FMath::Abs(BallVelocity.X) > 175.f
                ? (KeeperPlaneX - BallPosition.X) / BallVelocity.X : -1.f;
            const bool bApproachingKeeper = TravelSeconds >= 0.f && TravelSeconds <= 1.5f;
            const float AnticipatedY = bApproachingKeeper
                ? BallPosition.Y + BallVelocity.Y * TravelSeconds : BallPosition.Y;
            Target.Y = FMath::Clamp(AnticipatedY, -300.f, 300.f);
            if (FVector::DistSquared2D(Target, Keeper->GetActorLocation()) > FMath::Square(60.f))
                MoveToLocation(Target, 60.f, true, true, false, true, nullptr, true);
            const float GoalwardSign = Keeper->TeamIndex == 1 ? 1.f : -1.f;
            const bool bIncomingPenalty = Ball->GetVelocity().X * GoalwardSign > 175.f;
            if (bIncomingPenalty && ActionCooldownRemaining <= 0.f &&
                FVector::DistSquared(Keeper->GetActorLocation(), Ball->GetActorLocation()) <
                    FMath::Square(ContactRangeCm + 50.f))
            {
                const FVector Parry(0.f, Ball->GetActorLocation().Y >= 0.f ? 1.f : -1.f, 0.25f);
                if (Ball->KickByPlayer(Keeper, Parry, 2300.f, ContactRangeCm + 50.f))
                    ActionCooldownRemaining = FMath::Max(ActionCooldownSeconds, 0.15f);
            }
            break;
        }
        return;
    }
    if (IsValid(Match) && (Match->bHalfTime || Match->bMatchFinished || Match->bBallOutOfPlay))
    {
        StopMovement();
        return;
    }

    ASportsArcadeCharacter* Footballer = Cast<ASportsArcadeCharacter>(GetPawn());
    if (!IsValid(Footballer)) return;

    ASportsArcadeBall* Ball = nullptr;
    for (TActorIterator<ASportsArcadeBall> It(GetWorld()); It; ++It)
    {
        if (!IsValid(*It)) continue;
        Ball = *It;
        break;
    }
    ASportsArcadeTeamRoster* Roster = nullptr;
    for (TActorIterator<ASportsArcadeTeamRoster> It(GetWorld()); It; ++It)
    {
        if (It->TeamIndex == Footballer->TeamIndex)
        {
            Roster = *It;
            break;
        }
    }
    if (!IsValid(Roster))
    {
        StopMovement();
        return;
    }

    // Match-phase movement should not inherit stale navigation requests from halftime.
    const int32 MembershipIndex = Roster->Players.IndexOfByKey(Footballer);
    const int32 Slot = Roster->bUseExplicitLineupSlots ? Footballer->LineupSlot : MembershipIndex;
    if (MembershipIndex == INDEX_NONE || Slot < 0 || Slot > 10 ||
        Footballer->TeamIndex < 0 || Footballer->TeamIndex > 1)
    {
        StopMovement();
        return;
    }
    FVector Target = Roster->GetFormationPosition(Slot);
    const float HalfLength = IsValid(Match) ? Match->PitchHalfLengthCm : 5250.f;
    const float HalfWidth = IsValid(Match) ? Match->PitchHalfWidthCm : 3400.f;
    Target.X *= HalfLength / 5250.f;
    Target.Y *= HalfWidth / 3400.f;
    bool bIsPrimaryPresser = false;
    if (IsValid(Ball) && Slot == 0)
    {
        // Sweeper-keeper starter positioning: lateral tracking and bounded advance.
        const float AnticipatedY = Ball->GetActorLocation().Y + Ball->GetVelocity().Y * FMath::Clamp(GoalkeeperAnticipationSeconds, 0.f, 1.f);
        Target.Y = FMath::Clamp(AnticipatedY * 0.20f, -GoalkeeperLateralRangeCm, GoalkeeperLateralRangeCm);
        const float TowardsPlay = Footballer->TeamIndex == 0 ? 1.f : -1.f;
        Target.X += TowardsPlay * FMath::Clamp((HalfLength - FMath::Abs(Ball->GetActorLocation().X)) * 0.12f, 0.f, GoalkeeperAdvanceCm);
        // Initial close-range keeper clearance; full diving/parry systems will replace this.
        const FVector BallVelocity = Ball->GetVelocity();
        const bool bIncomingShot = BallVelocity.X * TowardsPlay < -175.f;
        if (ActionCooldownRemaining <= 0.f &&
            FVector::DistSquared2D(Footballer->GetActorLocation(), Ball->GetActorLocation()) < FMath::Square(ContactRangeCm + 75.f) &&
            Ball->GetActorLocation().Z < 200.f &&
            FMath::Abs(Footballer->GetActorLocation().Z - Ball->GetActorLocation().Z) < 175.f)
        {
            // Near-goal incoming attempts get a stronger lateral parry; low-speed
            // contacts retain the existing forward clearance behavior.
            const float ParryY = Ball->GetActorLocation().Y >= 0.f ? 0.85f : -0.85f;
            const FVector Clearance = bIncomingShot
                ? FVector(TowardsPlay * 0.35f, ParryY, 0.35f)
                : FVector(TowardsPlay, ParryY * 0.45f, 0.25f);
            const float Impulse = bIncomingShot ? 2850.f : 2300.f;
            if (Ball->KickByPlayer(Footballer, Clearance, Impulse, ContactRangeCm + 75.f))
            {
                ActionCooldownRemaining = FMath::Max(ActionCooldownSeconds, 0.15f);
            }
        }
    }
    else if (IsValid(Ball))
    {
        // Keep team shape while shifting towards the current phase of play.
        const float Advance = FMath::Clamp(FormationAdvanceFactor, 0.f, 0.65f);
        Target.X += FMath::Clamp(Ball->GetActorLocation().X - Target.X, -2200.f, 2200.f) * Advance;
        Target.Y += FMath::Clamp(Ball->GetActorLocation().Y - Target.Y, -1800.f, 1800.f) * Advance;

        // Only the closest eligible outfielder presses; others maintain support shape.
        ASportsArcadeCharacter* Nearest = nullptr;
        float BestDistance = TNumericLimits<float>::Max();
        for (ASportsArcadeCharacter* Candidate : Roster->Players)
        {
            // A human-controlled footballer is not an available autonomous
            // presser; otherwise every nearby AI teammate could stay passive.
            if (!IsValid(Candidate) || Candidate->GetWorld() != GetWorld() ||
                Candidate->TeamIndex != Footballer->TeamIndex || Candidate->LineupSlot == 0 ||
                !Cast<ASportsArcadeTeamAIController>(Candidate->GetController())) continue;
            const float Distance = FVector::DistSquared2D(Candidate->GetActorLocation(), Ball->GetActorLocation());
            if (Distance < BestDistance)
            {
                BestDistance = Distance;
                Nearest = Candidate;
            }
        }
        if (Nearest == Footballer && BestDistance <= FMath::Square(PressureRadiusCm))
        {
            bIsPrimaryPresser = true;
            // Lead moving balls slightly for more credible interception movement.
            FVector Predicted = Ball->GetActorLocation() + Ball->GetVelocity() * FMath::Clamp(BallPredictionSeconds, 0.f, 0.5f);
            Target = FVector(Predicted.X, Predicted.Y, 0.f);
            if (BestDistance <= FMath::Square(ContactRangeCm) && ActionCooldownRemaining <= 0.f &&
                FMath::Abs(Footballer->GetActorLocation().Z - Ball->GetActorLocation().Z) < 135.f)
            {
                // Prefer a forward teammate with a clear passing lane; otherwise clear towards goal.
                const float AttackSign = Footballer->TeamIndex == 0 ? 1.f : -1.f;
                FVector Direction = FVector(AttackSign * HalfLength - Ball->GetActorLocation().X,
                    -Ball->GetActorLocation().Y, 0.f).GetSafeNormal();
                float BestPassScore = -BIG_NUMBER;
                for (ASportsArcadeCharacter* Teammate : Roster->Players)
                {
                    if (!IsValid(Teammate) || Teammate->GetWorld() != GetWorld() ||
                        Teammate == Footballer || Teammate->TeamIndex != Footballer->TeamIndex) continue;
                    const FVector Delta = Teammate->GetActorLocation() - Ball->GetActorLocation();
                    const float Distance = Delta.Size2D();
                    // Support lateral circulation and bounded back-passes when no
                    // progressive lane is available; score still favours forward play.
                    if (Distance < 300.f || Distance > 3300.f ||
                        Delta.X * AttackSign < -650.f) continue;
                    bool bBlocked = false;
                    float ReceiverPressurePenalty = 0.f;
                    for (TActorIterator<ASportsArcadeCharacter> Opp(GetWorld()); Opp; ++Opp)
                    {
                        if (!IsValid(*Opp) || Opp->TeamIndex == Footballer->TeamIndex) continue;
                        // An unblocked pass can still be intercepted at the receiver.
                        const float ReceiverSeparation = FVector::Dist2D(
                            Opp->GetActorLocation(), Teammate->GetActorLocation());
                        if (ReceiverSeparation < 210.f)
                        {
                            bBlocked = true;
                            break;
                        }
                        ReceiverPressurePenalty = FMath::Max(ReceiverPressurePenalty,
                            FMath::Max(0.f, 450.f - ReceiverSeparation) * 0.8f);
                        const FVector2D Start(Ball->GetActorLocation());
                        const FVector2D End(Teammate->GetActorLocation());
                        const FVector2D Segment = End - Start;
                        const float Alpha = FMath::Clamp(FVector2D::DotProduct(FVector2D(Opp->GetActorLocation()) - Start, Segment) /
                            FMath::Max(Segment.SizeSquared(), 1.f), 0.f, 1.f);
                        if (FVector2D::DistSquared(FVector2D(Opp->GetActorLocation()), Start + Segment * Alpha) < FMath::Square(170.f))
                        {
                            bBlocked = true;
                            break;
                        }
                    }
                    if (bBlocked) continue;
                    const float Score = Delta.X * AttackSign - Distance * 0.25f - ReceiverPressurePenalty;
                    if (Score > BestPassScore)
                    {
                        BestPassScore = Score;
                        Direction = Delta.GetSafeNormal2D();
                    }
                }
                if (Ball->KickByPlayer(Footballer, Direction, BestPassScore > -BIG_NUMBER ? 1400.f : 1700.f, ContactRangeCm + 25.f))
                {
                    // Count only an attacking-third direct goal attempt, not a
                    // normal pass or deep defensive clearance.
                    if (IsValid(Match) && BestPassScore <= -BIG_NUMBER &&
                        Ball->GetActorLocation().X * AttackSign > 1500.f)
                    {
                        Match->RecordShotAttempt(Footballer->TeamIndex);
                    }
                    ActionCooldownRemaining = FMath::Max(ActionCooldownSeconds, 0.15f);
                }
            }
        }
    }
    // A conservative defensive shadow towards the closest nearby opponent.
    // Pressing footballer already has the ball as target and must not be pulled away.
    if (Slot != 0 && !bIsPrimaryPresser && IsValid(Ball) && FVector::DistSquared2D(Target, Ball->GetActorLocation()) > FMath::Square(200.f))
    {
        ASportsArcadeCharacter* Opponent = nullptr;
        float NearestOpponentDistance = FMath::Square(FMath::Max(0.f, MarkingSearchRadiusCm));
        for (TActorIterator<ASportsArcadeCharacter> It(GetWorld()); It; ++It)
        {
            if (!IsValid(*It) || It->TeamIndex == Footballer->TeamIndex) continue;
            const float Distance = FVector::DistSquared2D(Footballer->GetActorLocation(), It->GetActorLocation());
            if (Distance < NearestOpponentDistance)
            {
                Opponent = *It;
                NearestOpponentDistance = Distance;
            }
        }
        if (IsValid(Opponent))
        {
            const FVector MarkingPoint = Opponent->GetActorLocation();
            Target = FMath::Lerp(Target, FVector(MarkingPoint.X, MarkingPoint.Y, 0.f),
                FMath::Clamp(MarkingInfluence, 0.f, 0.5f));
        }
    }
    Target.X = FMath::Clamp(Target.X, -HalfLength + 150.f, HalfLength - 150.f);
    Target.Y = FMath::Clamp(Target.Y, -HalfWidth + 150.f, HalfWidth - 150.f);
    // Avoid submitting an identical path request when already inside the acceptance radius.
    if (FVector::DistSquared2D(Footballer->GetActorLocation(), Target) <= FMath::Square(125.f))
    {
        StopMovement();
        return;
    }
    MoveToLocation(Target, 125.f, true, true, false, true, nullptr, true);
}
