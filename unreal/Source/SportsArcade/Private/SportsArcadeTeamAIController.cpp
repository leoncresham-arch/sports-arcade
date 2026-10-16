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
    DecisionCountdown -= DeltaSeconds;
    ActionCooldownRemaining = FMath::Max(0.f, ActionCooldownRemaining - DeltaSeconds);
    if (DecisionCountdown > 0.f || !GetWorld()) return;
    DecisionCountdown = FMath::Max(DecisionIntervalSeconds, 0.05f);

    const ASportsArcadeGameMode* Match = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this));
    if (IsValid(Match) && (Match->bHalfTime || Match->bMatchFinished))
    {
        StopMovement();
        return;
    }

    ASportsArcadeCharacter* Footballer = Cast<ASportsArcadeCharacter>(GetPawn());
    if (!IsValid(Footballer)) return;

    ASportsArcadeBall* Ball = nullptr;
    for (TActorIterator<ASportsArcadeBall> It(GetWorld()); It; ++It)
    {
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
    if (!IsValid(Roster)) return;

    const int32 Slot = Roster->Players.IndexOfByKey(Footballer);
    if (Slot == INDEX_NONE) return;
    FVector Target = Roster->GetFormationPosition(Slot);
    if (IsValid(Ball) && Slot == 0)
    {
        // Sweeper-keeper starter positioning: lateral tracking and bounded advance.
        Target.Y = FMath::Clamp(Ball->GetActorLocation().Y * 0.20f, -GoalkeeperLateralRangeCm, GoalkeeperLateralRangeCm);
        const float TowardsPlay = Footballer->TeamIndex == 0 ? 1.f : -1.f;
        Target.X += TowardsPlay * FMath::Clamp((5250.f - FMath::Abs(Ball->GetActorLocation().X)) * 0.12f, 0.f, GoalkeeperAdvanceCm);
        // Initial close-range keeper clearance; full diving/parry systems will replace this.
        if (ActionCooldownRemaining <= 0.f &&
            FVector::DistSquared2D(Footballer->GetActorLocation(), Ball->GetActorLocation()) < FMath::Square(ContactRangeCm + 75.f) &&
            Ball->GetActorLocation().Z < 200.f &&
            FMath::Abs(Footballer->GetActorLocation().Z - Ball->GetActorLocation().Z) < 175.f)
        {
            const FVector Clearance = FVector(TowardsPlay, (Ball->GetActorLocation().Y >= 0.f ? 0.4f : -0.4f), 0.25f);
            Ball->Kick(Clearance, 2300.f);
            ActionCooldownRemaining = FMath::Max(ActionCooldownSeconds, 0.15f);
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
        for (int32 Index = 1; Index < Roster->Players.Num(); ++Index)
        {
            ASportsArcadeCharacter* Candidate = Roster->Players[Index];
            if (!IsValid(Candidate) || Candidate->TeamIndex != Footballer->TeamIndex) continue;
            const float Distance = FVector::DistSquared2D(Candidate->GetActorLocation(), Ball->GetActorLocation());
            if (Distance < BestDistance)
            {
                BestDistance = Distance;
                Nearest = Candidate;
            }
        }
        if (Nearest == Footballer && BestDistance <= FMath::Square(PressureRadiusCm))
        {
            // Lead moving balls slightly for more credible interception movement.
            FVector Predicted = Ball->GetActorLocation() + Ball->GetVelocity() * FMath::Clamp(BallPredictionSeconds, 0.f, 0.5f);
            Target = FVector(Predicted.X, Predicted.Y, 0.f);
            if (BestDistance <= FMath::Square(ContactRangeCm) && ActionCooldownRemaining <= 0.f &&
                FMath::Abs(Footballer->GetActorLocation().Z - Ball->GetActorLocation().Z) < 135.f)
            {
                // Prefer a forward teammate with a clear passing lane; otherwise clear towards goal.
                const float AttackSign = Footballer->TeamIndex == 0 ? 1.f : -1.f;
                FVector Direction = FVector(AttackSign * 5250.f - Ball->GetActorLocation().X,
                    -Ball->GetActorLocation().Y, 0.f).GetSafeNormal();
                float BestPassScore = -BIG_NUMBER;
                for (ASportsArcadeCharacter* Teammate : Roster->Players)
                {
                    if (!IsValid(Teammate) || Teammate == Footballer || Teammate->TeamIndex != Footballer->TeamIndex) continue;
                    const FVector Delta = Teammate->GetActorLocation() - Ball->GetActorLocation();
                    const float Distance = Delta.Size2D();
                    if (Distance < 300.f || Distance > 3300.f || Delta.X * AttackSign < 150.f) continue;
                    bool bBlocked = false;
                    for (TActorIterator<ASportsArcadeCharacter> Opp(GetWorld()); Opp; ++Opp)
                    {
                        if (Opp->TeamIndex == Footballer->TeamIndex) continue;
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
                    const float Score = Delta.X * AttackSign - Distance * 0.25f;
                    if (Score > BestPassScore)
                    {
                        BestPassScore = Score;
                        Direction = Delta.GetSafeNormal2D();
                    }
                }
                Ball->Kick(Direction, BestPassScore > -BIG_NUMBER ? 1400.f : 1700.f);
                ActionCooldownRemaining = FMath::Max(ActionCooldownSeconds, 0.15f);
            }
        }
    }
    // A conservative defensive shadow towards the closest nearby opponent.
    // Pressing footballer already has the ball as target and must not be pulled away.
    if (Slot != 0 && IsValid(Ball) && FVector::DistSquared2D(Target, Ball->GetActorLocation()) > FMath::Square(200.f))
    {
        ASportsArcadeCharacter* Opponent = nullptr;
        float NearestOpponentDistance = FMath::Square(FMath::Max(0.f, MarkingSearchRadiusCm));
        for (TActorIterator<ASportsArcadeCharacter> It(GetWorld()); It; ++It)
        {
            if (It->TeamIndex == Footballer->TeamIndex) continue;
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
    Target.X = FMath::Clamp(Target.X, -5100.f, 5100.f);
    Target.Y = FMath::Clamp(Target.Y, -3250.f, 3250.f);
    MoveToLocation(Target, 125.f, true, true, false, true, nullptr, true);
}
