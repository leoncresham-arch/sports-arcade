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
        if (Match->GetSuggestedCupPenaltyOutcome() != INDEX_NONE)
        {
            StopMovement();
            return;
        }
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
            if (BallPosition.ContainsNaN() || BallVelocity.ContainsNaN())
            {
                StopMovement();
                break;
            }
            const float KeeperPlaneX = Keeper->GetActorLocation().X;
            const float GoalwardSign = Keeper->TeamIndex == 1 ? 1.f : -1.f;
            const bool bIncomingPenalty = BallVelocity.X * GoalwardSign >
                FMath::Max(0.f, GoalkeeperShotMinSpeedCmPerSec);
            const float TravelSeconds = bIncomingPenalty
                ? (KeeperPlaneX - BallPosition.X) / BallVelocity.X : -1.f;
            const bool bApproachingKeeper = TravelSeconds >= 0.f && TravelSeconds <= 1.5f;
            const float AnticipatedY = bApproachingKeeper
                ? BallPosition.Y + BallVelocity.Y * TravelSeconds : 0.f;
            // Remain close to the line until the shot is actually heading here.
            const float LateralLimit = FMath::Min(300.f, FMath::Max(0.f, Match->GoalHalfWidthCm - 55.f));
            Target.Y = FMath::Clamp(AnticipatedY, -LateralLimit, LateralLimit);
            if (FVector::DistSquared2D(Target, Keeper->GetActorLocation()) > FMath::Square(60.f))
                MoveToLocation(Target, 60.f, true, true, false, true, nullptr, true);
            // Do not parry a ball that has already passed the keeper's goal
            // plane, or a speculative ball outside plausible save height.
            const bool bBallInFront = (BallPosition.X - KeeperPlaneX) * GoalwardSign <=
                FMath::Max(0.f, GoalkeeperParryOverrunAllowanceCm);
            const bool bReachableHeight = BallPosition.Z >= 0.f &&
                BallPosition.Z - Keeper->GetActorLocation().Z <= FMath::Max(0.f, GoalkeeperParryHeightCm);
            if (bIncomingPenalty && bApproachingKeeper && bBallInFront && bReachableHeight &&
                ActionCooldownRemaining <= 0.f &&
                FVector::DistSquared(Keeper->GetActorLocation(), Ball->GetActorLocation()) <
                    FMath::Square(ContactRangeCm + 50.f))
            {
                // Direct the rebound toward a sideline, away from central danger.
                const FVector Parry(0.f, BallPosition.Y >= 0.f ? 1.f : -1.f, 0.25f);
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
    // Reject nonfinite physical telemetry before formation, pressing, marking
    // and pass-choice logic can submit AI navigation or physics impulses.
    if (IsValid(Ball) && (Ball->GetActorLocation().ContainsNaN() ||
        Ball->GetVelocity().ContainsNaN()))
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
    bool bIsSecondaryPresser = false;
    if (IsValid(Ball) && Slot == 0)
    {
        // Never issue a navigation target from nonfinite physics telemetry.
        const FVector BallPosition = Ball->GetActorLocation();
        const FVector RawBallVelocity = Ball->GetVelocity();
        if (BallPosition.ContainsNaN() || RawBallVelocity.ContainsNaN())
        {
            StopMovement();
            return;
        }
        const FVector BallVelocity = RawBallVelocity.GetClampedToMaxSize(4500.f);
        // Track approaching shots more decisively while retaining a conservative
        // central stance for ordinary midfield possession.
        const float TowardsPlay = Footballer->TeamIndex == 0 ? 1.f : -1.f;
        const bool bIncomingShot = BallVelocity.X * TowardsPlay <
            -FMath::Max(0.f, GoalkeeperShotMinSpeedCmPerSec);
        const float ApproachSeconds = bIncomingShot &&
            FMath::Abs(BallVelocity.X) > KINDA_SMALL_NUMBER
            ? (Footballer->GetActorLocation().X - BallPosition.X) / BallVelocity.X : -1.f;
        const bool bPredictedArrival = ApproachSeconds >= 0.f && ApproachSeconds <= 1.5f;
        const float AnticipatedY = bPredictedArrival
            ? BallPosition.Y + BallVelocity.Y * ApproachSeconds
            : (BallPosition.Y + BallVelocity.Y * FMath::Clamp(GoalkeeperAnticipationSeconds, 0.f, 1.f)) * 0.20f;
        const float LateralRange = FMath::Min(FMath::Max(0.f, GoalkeeperLateralRangeCm),
            FMath::Max(0.f, IsValid(Match) ? Match->GoalHalfWidthCm - 55.f : 311.f));
        Target.Y = FMath::Clamp(AnticipatedY, -LateralRange, LateralRange);
        Target.X += TowardsPlay * FMath::Clamp((HalfLength - FMath::Abs(BallPosition.X)) * 0.12f, 0.f, GoalkeeperAdvanceCm);
        // Initial close-range keeper clearance; full diving/parry systems will replace this.
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
        // Last-touch attribution is a weak possession signal, not a guarantee:
        // use only small shape adjustments, never override primary pressing.
        // A last touch without nearby control is a loose ball, not possession.
        const ASportsArcadeCharacter* LastToucher = Ball->GetLastTouchPlayer();
        const bool bAttributedControl = IsValid(LastToucher) &&
            LastToucher->GetWorld() == GetWorld() &&
            !LastToucher->GetActorLocation().ContainsNaN() &&
            // Aerial and separated loose balls must not masquerade as
            // retained team possession based solely on touch history.
            FMath::Abs(LastToucher->GetActorLocation().Z -
                Ball->GetActorLocation().Z) <= 135.f &&
            FVector::DistSquared2D(LastToucher->GetActorLocation(),
                Ball->GetActorLocation()) <= FMath::Square(220.f);
        const int32 RecentTouchTeam = bAttributedControl
            ? Ball->GetLastTouchTeamIndex() : INDEX_NONE;
        const float AttackSign = Footballer->TeamIndex == 0 ? 1.f : -1.f;
        if (RecentTouchTeam == Footballer->TeamIndex)
            Target.X += AttackSign * FMath::Clamp(AttackingSupportShiftCm, 0.f, 500.f);
        else if (RecentTouchTeam == 1 - Footballer->TeamIndex)
            Target.X -= AttackSign * FMath::Clamp(DefensiveRecoveryShiftCm, 0.f, 500.f);

        // Forward players make a modest run beyond the ball during attributed
        // attacking possession. Keep defenders/midfield formation unchanged.
        if (RecentTouchTeam == Footballer->TeamIndex && Slot >= 9 && Slot <= 10 &&
            Ball->GetActorLocation().X * AttackSign > 0.f &&
            Footballer->GetActorLocation().X * AttackSign <
                Ball->GetActorLocation().X * AttackSign + 1200.f)
        {
            Target.X += AttackSign * FMath::Clamp(ForwardRunOffsetCm, 0.f, 1500.f);
            // Two advanced runners approach from different channels.
            Target.Y += (Slot == 9 ? -1.f : 1.f) *
                FMath::Clamp(ForwardRunLaneSeparationCm, 0.f, 800.f);
        }

        // Central midfielders supply a short forward passing option when our
        // ball carrier enters the attacking half, but never sprint beyond the
        // ball or abandon their base formation for a distant possession.
        if (RecentTouchTeam == Footballer->TeamIndex && Slot >= 5 && Slot <= 8 &&
            Ball->GetActorLocation().X * AttackSign > 0.f &&
            FVector::DistSquared2D(Footballer->GetActorLocation(),
                Ball->GetActorLocation()) < FMath::Square(2400.f))
        {
            const float SupportGap = (Ball->GetActorLocation().X - Target.X) * AttackSign;
            Target.X += AttackSign * FMath::Clamp(SupportGap * 0.25f, 0.f, 450.f);
            // The supporting midfielder should not overrun the available
            // passing lane and stand farther forward than the ball carrier.
            const float MaximumSupportX = Ball->GetActorLocation().X -
                AttackSign * 175.f;
            Target.X = AttackSign > 0.f
                ? FMath::Min(Target.X, MaximumSupportX)
                : FMath::Max(Target.X, MaximumSupportX);
            // Preserve a useful passing triangle: midfield support near the
            // carrier spreads into the opposite channel without vacating
            // its base formation completely.
            const float LateralSeparation = Target.Y - Ball->GetActorLocation().Y;
            if (FMath::Abs(LateralSeparation) < 250.f)
            {
                const float SideSign = Slot % 2 == 0 ? 1.f : -1.f;
                Target.Y += SideSign * FMath::Min(250.f - FMath::Abs(LateralSeparation), 250.f);
            }
        }

        // Hold the authored back line within a tunable attacking/recovery band.
        if (Slot >= 1 && Slot <= 4 && RecentTouchTeam == 1 - Footballer->TeamIndex)
        {
            const FVector BasePosition = Roster->GetFormationPosition(Slot);
            const float BaseX = BasePosition.X * HalfLength / 5250.f;
            const float Advance = FMath::Clamp(DefensiveLineMaximumAdvanceCm, 0.f, 2500.f);
            const float Retreat = FMath::Clamp(DefensiveLineRecoveryDepthCm, 0.f, 2000.f);
            const float RelativeX = (Target.X - BaseX) * AttackSign;
            Target.X = BaseX + AttackSign * FMath::Clamp(RelativeX, -Retreat, Advance);
            const float BaseY = BasePosition.Y * HalfWidth / 3400.f;
            const float LateralLimit = FMath::Clamp(DefensiveLineLateralDisplacementCm, 0.f, 1500.f);
            Target.Y = FMath::Clamp(Target.Y, BaseY - LateralLimit, BaseY + LateralLimit);
        }

        // Make nearby non-keeper support runners offer a clear lateral outlet.
        // Shift away from the nearest opponent only when our team last touched
        // the ball; cap the adjustment to retain the authored formation.
        if (RecentTouchTeam == Footballer->TeamIndex &&
            FVector::DistSquared2D(Footballer->GetActorLocation(), Ball->GetActorLocation()) <
                FMath::Square(2400.f))
        {
            float ClosestOpponentSq = FMath::Square(FMath::Clamp(PassingSupportOpponentSearchCm, 0.f, 2000.f));
            const ASportsArcadeCharacter* NearestOpponent = nullptr;
            for (TActorIterator<ASportsArcadeCharacter> Opp(GetWorld()); Opp; ++Opp)
            {
                if (!IsValid(*Opp) || Opp->GetWorld() != GetWorld() ||
                    Opp->TeamIndex == Footballer->TeamIndex ||
                    Opp->GetActorLocation().ContainsNaN()) continue;
                const float DistanceSq = FVector::DistSquared2D(Opp->GetActorLocation(), Target);
                if (DistanceSq < ClosestOpponentSq)
                {
                    ClosestOpponentSq = DistanceSq;
                    NearestOpponent = *Opp;
                }
            }
            if (NearestOpponent)
            {
                const float Side = Target.Y >= NearestOpponent->GetActorLocation().Y ? 1.f : -1.f;
                Target.Y += Side * FMath::Clamp(PassingSupportLateralShiftCm, 0.f, 1200.f);
            }
        }

        // Only the closest eligible outfielder presses; others maintain support shape.
        ASportsArcadeCharacter* Nearest = nullptr;
        float BestDistance = TNumericLimits<float>::Max();
        for (ASportsArcadeCharacter* Candidate : Roster->Players)
        {
            // A human-controlled footballer is not an available autonomous
            // presser; otherwise every nearby AI teammate could stay passive.
            if (!IsValid(Candidate) || Candidate->GetWorld() != GetWorld() ||
                Candidate->TeamIndex != Footballer->TeamIndex ||
                (Roster->bUseExplicitLineupSlots ? Candidate->LineupSlot == 0 :
                    Roster->Players.IndexOfByKey(Candidate) == 0) ||
                !Cast<ASportsArcadeTeamAIController>(Candidate->GetController()) ||
                Candidate->GetActorLocation().ContainsNaN()) continue;
            const float Distance = FVector::DistSquared2D(Candidate->GetActorLocation(), Ball->GetActorLocation());
            if (Distance < BestDistance)
            {
                BestDistance = Distance;
                Nearest = Candidate;
            }
        }
        // The second-closest autonomous outfielder supports defensive pressure
        // at a conservative stand-off. Avoid both defenders chasing one point.
        // Back-line defenders support a second press only when the ball is
        // deep in their own half; midfielders and attackers may press higher.
        const bool bDefensiveEmergency = Ball->GetActorLocation().X * AttackSign < -HalfLength * 0.45f;
        if (Nearest != Footballer && RecentTouchTeam == 1 - Footballer->TeamIndex &&
            (Slot > 4 || bDefensiveEmergency))
        {
            ASportsArcadeCharacter* Second = nullptr;
            float SecondDistance = TNumericLimits<float>::Max();
            for (ASportsArcadeCharacter* Candidate : Roster->Players)
            {
                if (!IsValid(Candidate) || Candidate == Nearest ||
                    Candidate->GetWorld() != GetWorld() ||
                    Candidate->TeamIndex != Footballer->TeamIndex ||
                    (Roster->bUseExplicitLineupSlots ? Candidate->LineupSlot == 0 :
                        Roster->Players.IndexOfByKey(Candidate) == 0) ||
                    !Cast<ASportsArcadeTeamAIController>(Candidate->GetController()) ||
                    Candidate->GetActorLocation().ContainsNaN()) continue;
                const float Distance = FVector::DistSquared2D(
                    Candidate->GetActorLocation(), Ball->GetActorLocation());
                if (Distance < SecondDistance)
                {
                    SecondDistance = Distance;
                    Second = Candidate;
                }
            }
            if (Second == Footballer &&
                SecondDistance <= FMath::Square(FMath::Clamp(SecondaryPressureRadiusCm, 0.f, 3000.f)))
            {
                const float StandOff = FMath::Clamp(SecondaryPressureStandOffCm, 0.f, 1000.f);
                // Move with the ball's short-term path instead of repeatedly
                // defending its previous position as the primary presser closes.
                const FVector SafeBallVelocity = Ball->GetVelocity().ContainsNaN()
                    ? FVector::ZeroVector : Ball->GetVelocity().GetClampedToMaxSize2D(1600.f);
                const bool bAirborneSupport = FMath::Abs(Ball->GetActorLocation().Z -
                    Footballer->GetActorLocation().Z) > 150.f;
                const float SupportLeadSeconds = bAirborneSupport
                    ? (SafeBallVelocity.Z > 100.f ? 0.f : 0.10f)
                    : FMath::Clamp(BallPredictionSeconds, 0.f, 0.25f);
                const FVector BallLead = Ball->GetActorLocation() +
                    SafeBallVelocity * SupportLeadSeconds;
                if (BallLead.ContainsNaN()) { StopMovement(); return; }
                Target = BallLead - FVector(AttackSign * StandOff, 0.f, BallLead.Z);
                // The second defender approaches from the side of the play,
                // denying a simple sideways escape instead of mirroring the
                // first defender's straight-line route.
                // Shade towards the interior when the ball is near a sideline;
                // this prevents the supporting defender from inviting an easy
                // cut back into the central passing corridor.
                const bool bNearTouchline = FMath::Abs(BallLead.Y) > HalfWidth - 650.f;
                // Only treat a significant lateral run as inward travel.
                // Multiplying position by velocity previously made this gate
                // trigger on almost any tiny inward movement at wide positions.
                const bool bBallMovingInfield = FMath::Abs(BallLead.Y) > 500.f &&
                    BallLead.Y * SafeBallVelocity.Y < 0.f &&
                    FMath::Abs(SafeBallVelocity.Y) > 150.f;
                const float SideSign = (bNearTouchline || bBallMovingInfield)
                    ? (BallLead.Y >= 0.f ? -1.f : 1.f)
                    : (Footballer->GetActorLocation().Y >= BallLead.Y ? 1.f : -1.f);
                Target.Y += SideSign * FMath::Max(StandOff * 0.5f, 120.f);
                // Do not crowd the ball: maintain a minimum lateral shadow
                // while the primary presser executes the direct challenge.
                // Preserve defensive shadow position inside the playable area
                // even when pressure happens next to the touchline.
                Target.X = FMath::Clamp(Target.X, -HalfLength + 100.f, HalfLength - 100.f);
                Target.Y = FMath::Clamp(Target.Y, -HalfWidth + 100.f, HalfWidth - 100.f);
                bIsSecondaryPresser = true;
            }
        }
        // Do not press a teammate's clearly controlled ball. Last touch is
        // only a proxy, so retain pressure once the ball has separated.
        const bool bTeammateNearbyBall = RecentTouchTeam == Footballer->TeamIndex &&
            IsValid(Ball->GetLastTouchPlayer()) &&
            Ball->GetLastTouchPlayer() != Footballer &&
            Ball->GetLastTouchPlayer()->GetWorld() == GetWorld() &&
            !Ball->GetLastTouchPlayer()->GetActorLocation().ContainsNaN() &&
            FMath::Abs(Ball->GetLastTouchPlayer()->GetActorLocation().Z -
                Ball->GetActorLocation().Z) <= 115.f &&
            FVector::DistSquared2D(Ball->GetLastTouchPlayer()->GetActorLocation(),
                Ball->GetActorLocation()) < FMath::Square(220.f);
        // A controlled opponent ball deserves pressure; when the ball is loose,
        // the nearest defender still recovers it. The carrier never presses
        // its own controlled touch simply because it is nearest to the ball.
        const bool bSelfControllingBall = RecentTouchTeam == Footballer->TeamIndex &&
            Ball->GetLastTouchPlayer() == Footballer &&
            FVector::DistSquared2D(Footballer->GetActorLocation(),
                Ball->GetActorLocation()) <= FMath::Square(220.f);
        // Carry a nearby controlled ball toward goal with a measured touch.
        // Avoid repeated impulses until the existing action cooldown clears.
        if (bSelfControllingBall)
        {
            // Follow the ball carrier's attacking lane between controlled touches.
            Target = Ball->GetActorLocation() + FVector(AttackSign * 280.f, 0.f, -Ball->GetActorLocation().Z);
            bIsPrimaryPresser = true; // Retain carrier target through generic marking.
        }
        // Preserve the chosen lateral escape lane between cooldown-gated touches.
        float CarrierEscapeSide = 0.f;
        if (bSelfControllingBall)
        {
            float ThreatDistanceSquared = FMath::Square(500.f);
            for (TActorIterator<ASportsArcadeCharacter> Threat(GetWorld()); Threat; ++Threat)
            {
                if (!IsValid(*Threat) || Threat->GetWorld() != GetWorld() ||
                    Threat->TeamIndex == Footballer->TeamIndex ||
                    Threat->GetActorLocation().ContainsNaN()) continue;
                const FVector ThreatOffset = Threat->GetActorLocation() - Ball->GetActorLocation();
                if (ThreatOffset.X * AttackSign < -100.f) continue;
                const float DistanceSquared = ThreatOffset.SizeSquared2D();
                if (DistanceSquared < ThreatDistanceSquared)
                {
                    ThreatDistanceSquared = DistanceSquared;
                    CarrierEscapeSide = ThreatOffset.Y >= 0.f ? -1.f : 1.f;
                }
            }
            // Prefer the infield route if the selected escape risks the touchline.
            if (FMath::Abs(Ball->GetActorLocation().Y + CarrierEscapeSide * 350.f) >
                HalfWidth - 250.f)
            {
                CarrierEscapeSide = Ball->GetActorLocation().Y >= 0.f ? -1.f : 1.f;
            }
            Target.Y += CarrierEscapeSide * 180.f;
        }
        // Delay the controlled touch if a defender is directly in the running lane.
        // Navigation remains active while the footballer chooses an escape route.
        const bool bDribbleLaneCrowded = bSelfControllingBall &&
            CarrierEscapeSide != 0.f;
        // Under close frontal pressure, retain control instead of forcing
        // a dribble touch directly into a nearby opponent.
        bool bImmediateChallenge = false;
        if (bSelfControllingBall)
        {
            for (TActorIterator<ASportsArcadeCharacter> Defender(GetWorld()); Defender; ++Defender)
            {
                if (!IsValid(*Defender) || Defender->GetWorld() != GetWorld() ||
                    Defender->TeamIndex == Footballer->TeamIndex ||
                    Defender->GetActorLocation().ContainsNaN()) continue;
                const FVector Approach = Defender->GetActorLocation() - Ball->GetActorLocation();
                if (Approach.X * AttackSign >= 0.f && Approach.SizeSquared2D() < FMath::Square(FMath::Clamp(DribbleChallengeRadiusCm, 50.f, 400.f)))
                {
                    bImmediateChallenge = true;
                    break;
                }
            }
        }
        if (bSelfControllingBall && bImmediateChallenge && ActionCooldownRemaining <= 0.f &&
            FMath::Abs(Footballer->GetActorLocation().Z - Ball->GetActorLocation().Z) < 135.f)
        {
            ASportsArcadeCharacter* BestOutlet = nullptr;
            float BestOutletScore = -BIG_NUMBER;
            FVector BestOutletTarget = FVector::ZeroVector;
            for (ASportsArcadeCharacter* Mate : Roster->Players)
            {
                if (!IsValid(Mate) || Mate == Footballer ||
                    Mate->GetWorld() != GetWorld() ||
                    Mate->TeamIndex != Footballer->TeamIndex ||
                    Mate->GetActorLocation().ContainsNaN()) continue;
                const FVector MateVelocity = Mate->GetVelocity();
                if (MateVelocity.ContainsNaN()) continue;
                const FVector Delta = Mate->GetActorLocation() + MateVelocity.GetClampedToMaxSize2D(650.f) * 0.20f - Ball->GetActorLocation();
                const float DistanceSq = Delta.SizeSquared2D();
                // Reject a degenerate escape direction before scoring the outlet.
                if (Delta.ContainsNaN() || DistanceSq <= KINDA_SMALL_NUMBER) continue;
                const FVector PredictedOutlet = Ball->GetActorLocation() + Delta;
                if (PredictedOutlet.ContainsNaN() ||
                    FMath::Abs(PredictedOutlet.X) > HalfLength - 100.f ||
                    FMath::Abs(PredictedOutlet.Y) > HalfWidth - 100.f) continue;
                if (DistanceSq < FMath::Square(350.f) || DistanceSq > FMath::Square(2200.f)) continue;
                // Reject outlets already marked at very close range.
                bool bReceiverMarked = false;
                for (TActorIterator<ASportsArcadeCharacter> Opp(GetWorld()); Opp; ++Opp)
                {
                    if (!IsValid(*Opp) || Opp->GetWorld() != GetWorld() ||
                        Opp->TeamIndex == Footballer->TeamIndex ||
                        Opp->GetActorLocation().ContainsNaN()) continue;
                    const FVector2D Start(Ball->GetActorLocation());
                    const FVector2D End(Ball->GetActorLocation() + Delta);
                    const FVector2D Path = End - Start;
                    const float Along = FMath::Clamp(FVector2D::DotProduct(
                        FVector2D(Opp->GetActorLocation()) - Start, Path) /
                        FMath::Max(Path.SizeSquared(), 1.f), 0.f, 1.f);
                    if (FVector2D::DistSquared(FVector2D(Opp->GetActorLocation()),
                        Start + Path * Along) < FMath::Square(160.f))
                    {
                        bReceiverMarked = true;
                        break;
                    }
                }
                if (bReceiverMarked) continue;
                // Prefer teammates away from the touchline when choices are similar.
                const float ProjectedReceiverY = Ball->GetActorLocation().Y + Delta.Y;
                const float SidelinePenalty = FMath::Max(0.f,
                    FMath::Abs(ProjectedReceiverY) - (HalfWidth - 450.f)) * 0.5f;
                const float OutletScore = Delta.X * AttackSign * 0.35f -
                    FMath::Sqrt(DistanceSq) * 0.25f - SidelinePenalty;
                if (OutletScore > BestOutletScore)
                {
                    BestOutletScore = OutletScore;
                    BestOutlet = Mate;
                    BestOutletTarget = Ball->GetActorLocation() + Delta;
                }
            }
            if (IsValid(BestOutlet))
            {
                // Release to the same finite target screened during selection.
                const FVector ReleaseDelta = BestOutletTarget - Ball->GetActorLocation();
                const FVector Direction = ReleaseDelta.GetSafeNormal2D();
                const float OutletDistanceCm = ReleaseDelta.Size2D();
                const float OutletImpulse = FMath::Clamp(950.f + OutletDistanceCm * 0.22f, 1000.f, 1550.f);
                if (Ball->KickByPlayer(Footballer, Direction, OutletImpulse, ContactRangeCm + 75.f))
                    ActionCooldownRemaining = FMath::Max(ActionCooldownSeconds, 0.15f);
            }
        }
        if (bSelfControllingBall && !bImmediateChallenge && ActionCooldownRemaining <= 0.f &&
            FMath::Abs(Footballer->GetActorLocation().Z - Ball->GetActorLocation().Z) <= 115.f)
        {
            // Reuse the same threat direction as the carrier's movement target.
            const float SafeLateralSign = CarrierEscapeSide;
            // Slow the forward push near an obstacle rather than force a full-speed touch.
            const float DribbleSpeed = bDribbleLaneCrowded ? 280.f : 380.f;
            const FVector DribbleDirection(AttackSign, SafeLateralSign * 0.6f, 0.f);
            // Keep the runner's movement aligned with the chosen escape lane.
            if (Ball->ApplyControlledTouch(Footballer, DribbleDirection * DribbleSpeed, ContactRangeCm + 75.f))
                ActionCooldownRemaining = FMath::Max(ActionCooldownSeconds * 0.5f, 0.15f);
        }
        if (!bTeammateNearbyBall && !bSelfControllingBall &&
            Nearest == Footballer &&
            BestDistance <= FMath::Square(PressureRadiusCm))
        {
            bIsPrimaryPresser = true;
            // Lead moving balls slightly for more credible interception movement.
            // Predict an interception using ball speed and the presser's travel
            // capacity; avoid always chasing a fixed future time point.
            const float SeparationCm = FMath::Sqrt(BestDistance);
            const float PursuitSeconds = SeparationCm / FMath::Clamp(PresserTravelSpeedCmPerSec, 100.f, 1000.f);
            const float LeadSeconds = FMath::Clamp(
                PursuitSeconds, 0.f, FMath::Clamp(BallPredictionSeconds, 0.f, 0.5f));
            const FVector BallVelocity = Ball->GetVelocity().ContainsNaN() ? FVector::ZeroVector :
                Ball->GetVelocity().GetClampedToMaxSize(3500.f);
            // A sharply lofted ball should not drag a ground presser toward a
            // far extrapolated landing point as if the ball were rolling.
            const bool bAirborne = FMath::Abs(Ball->GetActorLocation().Z -
                Footballer->GetActorLocation().Z) > 150.f;
            // The aerial lead is useful only when the ball is descending; a
            // rising ball should keep defenders close to its current ground
            // projection rather than sending them toward an imagined landing.
            const float GroundLeadSeconds = bAirborne
                ? (BallVelocity.Z > 100.f ? 0.f : FMath::Min(LeadSeconds, 0.12f))
                : LeadSeconds;
            FVector Predicted = Ball->GetActorLocation() + BallVelocity * GroundLeadSeconds;
            if (Predicted.ContainsNaN()) { StopMovement(); return; }
            Target = FVector(FMath::Clamp(Predicted.X, -HalfLength + 100.f, HalfLength - 100.f),
                FMath::Clamp(Predicted.Y, -HalfWidth + 100.f, HalfWidth - 100.f), 0.f);
            // No controlled kick attempts while the ball is too high to reach.
            const bool bBallAtFootHeight = FMath::Abs(Footballer->GetActorLocation().Z -
                Ball->GetActorLocation().Z) < 135.f;
            if (BestDistance <= FMath::Square(ContactRangeCm) && ActionCooldownRemaining <= 0.f &&
                bBallAtFootHeight)
            {
                // Prefer a forward teammate with a clear passing lane; otherwise clear towards goal.
                const float AttackSign = Footballer->TeamIndex == 0 ? 1.f : -1.f;
                FVector Direction = FVector(AttackSign * HalfLength - Ball->GetActorLocation().X,
                    -Ball->GetActorLocation().Y, 0.f).GetSafeNormal();
                float BestPassScore = -BIG_NUMBER;
                float BestPassDistanceCm = 0.f;
                for (ASportsArcadeCharacter* Teammate : Roster->Players)
                {
                    if (!IsValid(Teammate) || Teammate->GetWorld() != GetWorld() ||
                        Teammate == Footballer || Teammate->TeamIndex != Footballer->TeamIndex ||
                        Teammate->GetActorLocation().ContainsNaN()) continue;
                    const FVector ReceiverVelocity = Teammate->GetVelocity();
                    if (ReceiverVelocity.ContainsNaN()) continue;
                    const FVector Delta = Teammate->GetActorLocation() + ReceiverVelocity.GetClampedToMaxSize2D(650.f) * 0.25f - Ball->GetActorLocation();
                    const float Distance = Delta.Size2D();
                    if (Delta.ContainsNaN() || Distance <= KINDA_SMALL_NUMBER) continue;
                    const FVector PredictedReceiver = Ball->GetActorLocation() + Delta;
                    // Never target a teammate's extrapolated position beyond the
                    // playable touchlines or goal lines.
                    if (PredictedReceiver.ContainsNaN() ||
                        FMath::Abs(PredictedReceiver.X) > HalfLength - 100.f ||
                        FMath::Abs(PredictedReceiver.Y) > HalfWidth - 100.f) continue;
                    // Support lateral circulation and bounded back-passes when no
                    // progressive lane is available; score still favours forward play.
                    if (Distance < 300.f || Distance > 3300.f ||
                        Delta.X * AttackSign < -650.f) continue;
                    bool bBlocked = false;
                    float ReceiverPressurePenalty = 0.f;
                    for (TActorIterator<ASportsArcadeCharacter> Opp(GetWorld()); Opp; ++Opp)
                    {
                        if (!IsValid(*Opp) || Opp->GetWorld() != GetWorld() ||
                            Opp->TeamIndex == Footballer->TeamIndex ||
                            Opp->GetActorLocation().ContainsNaN()) continue;
                        // An unblocked pass can still be intercepted at the receiver.
                        const float ReceiverSeparation = FVector::Dist2D(
                            Opp->GetActorLocation(), Ball->GetActorLocation() + Delta);
                        if (ReceiverSeparation < 210.f)
                        {
                            bBlocked = true;
                            break;
                        }
                        ReceiverPressurePenalty = FMath::Max(ReceiverPressurePenalty,
                            FMath::Max(0.f, 450.f - ReceiverSeparation) * 0.8f);
                        const FVector2D Start(Ball->GetActorLocation());
                        const FVector2D End(Ball->GetActorLocation() + Delta);
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
                    // Penalize risky receiving positions near the sideline.
                    const float ReceiverY = Ball->GetActorLocation().Y + Delta.Y;
                    const float WidePenalty = FMath::Max(0.f,
                        FMath::Abs(ReceiverY) - (HalfWidth - 400.f)) * 0.35f;
                    const float Score = Delta.X * AttackSign - Distance * 0.25f -
                        ReceiverPressurePenalty - WidePenalty;
                    if (Score > BestPassScore)
                    {
                        BestPassScore = Score;
                        BestPassDistanceCm = Distance;
                        Direction = Delta.GetSafeNormal2D();
                    }
                }
                const float PassImpulse = FMath::Clamp(900.f + BestPassDistanceCm * 0.24f, 1000.f, 1800.f);
                const float KickImpulse = BestPassScore > -BIG_NUMBER ? PassImpulse : 1700.f;
                if (Ball->KickByPlayer(Footballer, Direction, KickImpulse, ContactRangeCm + 25.f))
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
    if (Slot != 0 && !bIsPrimaryPresser && !bIsSecondaryPresser && IsValid(Ball) &&
        (Ball->GetLastTouchTeamIndex() != Footballer->TeamIndex ||
            !IsValid(Ball->GetLastTouchPlayer()) ||
            Ball->GetLastTouchPlayer()->GetActorLocation().ContainsNaN() ||
            FVector::DistSquared2D(Ball->GetLastTouchPlayer()->GetActorLocation(),
                Ball->GetActorLocation()) > FMath::Square(300.f)) &&
        FVector::DistSquared2D(Target, Ball->GetActorLocation()) > FMath::Square(200.f))
    {
        ASportsArcadeCharacter* Opponent = nullptr;
        float NearestOpponentDistance = FMath::Square(FMath::Clamp(MarkingSearchRadiusCm, 0.f, 3000.f));
        for (TActorIterator<ASportsArcadeCharacter> It(GetWorld()); It; ++It)
        {
            if (!IsValid(*It) || It->GetWorld() != GetWorld() ||
                It->TeamIndex == Footballer->TeamIndex ||
                It->GetActorLocation().ContainsNaN()) continue;
            const float MarkingDirection = Footballer->TeamIndex == 0 ? 1.f : -1.f;
            const bool bDeepDefensiveThreat =
                It->GetActorLocation().X * MarkingDirection < -HalfLength * 0.45f;
            if (!bDeepDefensiveThreat &&
                (It->GetActorLocation().X - Ball->GetActorLocation().X) * MarkingDirection < -900.f) continue;
            const float Distance = FVector::DistSquared2D(Footballer->GetActorLocation(), It->GetActorLocation());
            // Prefer threats near our goalmouth while retaining distance-based marking.
            const float GoalProximityCm = FMath::Abs(It->GetActorLocation().X + MarkingDirection * HalfLength);
            const bool bCentralGoalThreat = FMath::Abs(It->GetActorLocation().Y) < 1800.f;
            const float GoalDangerWeight = GoalProximityCm < 1600.f && bCentralGoalThreat ? 0.75f : 1.f;
            // Prefer actively advancing runs over stationary opposition when
            // competing nearby marking assignments have similar distances.
            const FVector ThreatVelocity = It->GetVelocity();
            const bool bDirectGoalRun = !ThreatVelocity.ContainsNaN() &&
                ThreatVelocity.X * MarkingDirection < -180.f &&
                GoalProximityCm < 2500.f;
            // Runs through the central channel pose greater immediate danger
            // than equally distant runs hugging the touchline.
            // Do not surrender a central runner to a distant opponent merely
            // because the distant player happens to be moving more quickly.
            const bool bCentralRunner = bDirectGoalRun &&
                FMath::Abs(It->GetActorLocation().Y) < 1000.f;
            const float CentralRunWeight = bCentralRunner ? 0.85f : 1.f;
            const float ThreatDistance = Distance * (bDeepDefensiveThreat ? 0.85f : 1.f) *
                GoalDangerWeight * (bDirectGoalRun ? 0.82f : 1.f) * CentralRunWeight;
            // Do not claim distant attackers outside the original marking
            // search radius just because urgency weighting lowers their score.
            if (Distance > FMath::Square(FMath::Clamp(MarkingSearchRadiusCm, 0.f, 3000.f))) continue;
            if (ThreatDistance < NearestOpponentDistance)
            {
                Opponent = *It;
                NearestOpponentDistance = ThreatDistance;
            }
        }
        if (IsValid(Opponent))
        {
            const FVector OpponentVelocity = Opponent->GetVelocity();
            const FVector MarkingPoint = Opponent->GetActorLocation() +
                (OpponentVelocity.ContainsNaN() ? FVector::ZeroVector : OpponentVelocity.GetClampedToMaxSize2D(650.f) * 0.20f);
            if (MarkingPoint.ContainsNaN()) { StopMovement(); return; }
            // Stay goalside of the moving attacker when recovering defensively.
            const float DefenderAttackSign = Footballer->TeamIndex == 0 ? 1.f : -1.f;
            const bool bRunnerAttackingGoal = !OpponentVelocity.ContainsNaN() &&
                OpponentVelocity.X * DefenderAttackSign < -150.f;
            const float BaseGoalSideOffset = FVector::Dist2D(Footballer->GetActorLocation(), MarkingPoint) * 0.12f;
            const float GoalSideOffset = FMath::Clamp(
                BaseGoalSideOffset + (bRunnerAttackingGoal ? 45.f : 0.f), 80.f, 180.f);
            // A fast diagonal run across the face of goal needs additional
            // infield cover even when the runner is still far from the centre.
            const bool bDiagonalInfieldRun = bRunnerAttackingGoal &&
                MarkingPoint.Y * OpponentVelocity.Y < 0.f &&
                FMath::Abs(OpponentVelocity.Y) > 140.f;
            // Bias marking toward the central lane when the attacker is wide,
            // denying an easy inward cut while retaining goalside cover.
            const float InfieldMarkingShift = FMath::Clamp(
                FMath::Abs(MarkingPoint.Y) - 1100.f, 0.f, 1800.f) * 0.14f;
            // Reduce exaggerated central drift close to our own goal:
            // prioritize tracking the immediate threat over generic lane cover.
            const float OwnGoalX = DefenderAttackSign > 0.f ? -HalfLength : HalfLength;
            const float GoalDistance = FMath::Abs(MarkingPoint.X - OwnGoalX);
            const float UrgentGoalCover = FMath::Clamp(GoalDistance / 1200.f, 0.35f, 1.f);
            const float CentralCoverY = MarkingPoint.Y -
                FMath::Sign(MarkingPoint.Y) * InfieldMarkingShift * UrgentGoalCover -
                (bDiagonalInfieldRun ? FMath::Sign(MarkingPoint.Y) * 75.f : 0.f);
            const FVector GoalSideMarkingPoint = MarkingPoint -
                FVector(DefenderAttackSign * GoalSideOffset, MarkingPoint.Y - CentralCoverY, 0.f);
            Target = FMath::Lerp(Target, FVector(GoalSideMarkingPoint.X, GoalSideMarkingPoint.Y, 0.f),
                FMath::Clamp(MarkingInfluence, 0.f, 0.5f));
        }
    }
    // Never forward invalid authored formation or marking targets to navigation.
    if (Target.ContainsNaN() || Footballer->GetActorLocation().ContainsNaN())
    {
        StopMovement();
        return;
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
