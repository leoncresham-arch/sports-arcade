#include "SportsArcadeTeamAIController.h"
#include "SportsArcadeTeamRoster.h"
#include "SportsArcadeCharacter.h"
#include "SportsArcadeBall.h"
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
    if (DecisionCountdown > 0.f || !GetWorld()) return;
    DecisionCountdown = FMath::Max(DecisionIntervalSeconds, 0.05f);

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
    if (IsValid(Ball) && Slot != 0)
    {
        // Keep team shape while shifting towards the current phase of play.
        const float Advance = FMath::Clamp(FormationAdvanceFactor, 0.f, 0.65f);
        Target.X += FMath::Clamp(Ball->GetActorLocation().X - Target.X, -2200.f, 2200.f) * Advance;
        Target.Y += FMath::Clamp(Ball->GetActorLocation().Y - Target.Y, -1800.f, 1800.f) * Advance;
    }
    Target.X = FMath::Clamp(Target.X, -5100.f, 5100.f);
    Target.Y = FMath::Clamp(Target.Y, -3250.f, 3250.f);
    MoveToLocation(Target, 125.f, true, true, false, true, nullptr, true);
}
