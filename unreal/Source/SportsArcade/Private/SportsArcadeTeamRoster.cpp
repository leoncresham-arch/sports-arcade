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

FVector ASportsArcadeTeamRoster::GetFormationPosition(int32 PlayerSlot) const
{
    // Lineup order: keeper, four defenders, midfielders, attackers.
    // Values are pitch-relative centimetres, suitable for spawn targets;
    // no players are moved automatically by this method.
    static const FVector2D Slots433[11] = {
        {-4750.f, 0.f}, {-3100.f, -2600.f}, {-3300.f, -850.f},
        {-3300.f, 850.f}, {-3100.f, 2600.f}, {-850.f, -1800.f},
        {-1200.f, 0.f}, {-850.f, 1800.f}, {2200.f, -2350.f},
        {2700.f, 0.f}, {2200.f, 2350.f}
    };
    static const FVector2D Slots442[11] = {
        {-4750.f, 0.f}, {-3100.f, -2600.f}, {-3300.f, -850.f},
        {-3300.f, 850.f}, {-3100.f, 2600.f}, {-850.f, -2600.f},
        {-1100.f, -850.f}, {-1100.f, 850.f}, {-850.f, 2600.f},
        {2500.f, -850.f}, {2500.f, 850.f}
    };
    if (PlayerSlot < 0 || PlayerSlot >= 11 || TeamIndex < 0 || TeamIndex > 1)
        return FVector::ZeroVector;

    const FVector2D Point = Formation == ESportsArcadeFormation::Formation442
        ? Slots442[PlayerSlot] : Slots433[PlayerSlot];
    const float Direction = TeamIndex == 0 ? 1.f : -1.f;
    return FVector(Point.X * Direction, Point.Y * Direction, 0.f);
}
