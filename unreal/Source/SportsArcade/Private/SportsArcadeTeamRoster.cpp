#include "SportsArcadeTeamRoster.h"
#include "SportsArcadeCharacter.h"

bool ASportsArcadeTeamRoster::AddPlayer(ASportsArcadeCharacter* Player)
{
    if (!IsValid(Player) || TeamIndex < 0 || TeamIndex > 1 || Player->TeamIndex != TeamIndex ||
        Players.Contains(Player) || Players.Num() >= 11)
    {
        return false;
    }
    Players.Add(Player);
    return true;
}

int32 ASportsArcadeTeamRoster::GetPlayerCount() const
{
    int32 ValidPlayers = 0;
    for (const ASportsArcadeCharacter* Player : Players)
    {
        if (IsValid(Player) && Player->TeamIndex == TeamIndex) ++ValidPlayers;
    }
    return ValidPlayers;
}

ASportsArcadeCharacter* ASportsArcadeTeamRoster::GetClosestPlayer(const FVector& Location) const
{
    ASportsArcadeCharacter* Closest = nullptr;
    float BestDistance = TNumericLimits<float>::Max();
    for (ASportsArcadeCharacter* Player : Players)
    {
        if (!IsValid(Player) || Player->TeamIndex != TeamIndex) continue;
        const float Distance = FVector::DistSquared2D(Player->GetActorLocation(), Location);
        if (Distance < BestDistance)
        {
            BestDistance = Distance;
            Closest = Player;
        }
    }
    return Closest;
}
