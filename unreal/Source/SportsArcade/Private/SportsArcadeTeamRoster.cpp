#include "SportsArcadeTeamRoster.h"
#include "SportsArcadeCharacter.h"
#include "SportsArcadeFootballDatabase.h"
#include "EngineUtils.h"
#include "Engine/World.h"

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

bool ASportsArcadeTeamRoster::HasCompleteLineup() const
{
    if (TeamIndex < 0 || TeamIndex > 1 || Players.Num() != 11) return false;
    bool Slots[11] = {};
    for (const ASportsArcadeCharacter* Player : Players)
    {
        if (!IsValid(Player) || Player->TeamIndex != TeamIndex ||
            Player->LineupSlot < 0 || Player->LineupSlot >= 11 || Slots[Player->LineupSlot]) return false;
        Slots[Player->LineupSlot] = true;
    }
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

void ASportsArcadeTeamRoster::BeginPlay()
{
    Super::BeginPlay();
    if (bDiscoverPlacedPlayers) DiscoverPlacedPlayers();
}

void ASportsArcadeTeamRoster::DiscoverPlacedPlayers()
{
    if (!GetWorld()) return;
    const bool bRosterWasEmpty = Players.IsEmpty();
    for (TActorIterator<ASportsArcadeCharacter> It(GetWorld()); It; ++It)
    {
        if (Players.Num() >= 11) break;
        if (It->TeamIndex == TeamIndex) AddPlayer(*It);
    }
    if (bRosterWasEmpty)
    {
        // Stable discovery ordering: shirt number first, actor name for ties.
        Players.Sort([](const TObjectPtr<ASportsArcadeCharacter>& A,
                        const TObjectPtr<ASportsArcadeCharacter>& B)
        {
            if (!IsValid(A)) return false;
            if (!IsValid(B)) return true;
            if (A->ShirtNumber != B->ShirtNumber) return A->ShirtNumber < B->ShirtNumber;
            return A->GetName() < B->GetName();
        });
    }
}

bool ASportsArcadeTeamRoster::AssignFootballerRecords(const USportsArcadeFootballDatabase* Catalog)
{
    if (!IsValid(Catalog) || ClubId.IsNone() || !HasCompleteLineup()) return false;
    TMap<int32, FName> SquadByShirt;
    for (const FSportsArcadeFootballerRecord& Record : Catalog->Footballers)
    {
        if (Record.ClubId != ClubId) continue;
        if (Record.Id.IsNone() || Record.ShirtNumber < 1 || Record.ShirtNumber > 99 ||
            SquadByShirt.Contains(Record.ShirtNumber)) return false;
        SquadByShirt.Add(Record.ShirtNumber, Record.Id);
    }
    // Check all assignments before changing any live player.
    TSet<int32> UsedShirts;
    for (const ASportsArcadeCharacter* Player : Players)
    {
        if (!IsValid(Player) || !SquadByShirt.Contains(Player->ShirtNumber) ||
            UsedShirts.Contains(Player->ShirtNumber)) return false;
        UsedShirts.Add(Player->ShirtNumber);
    }
    for (ASportsArcadeCharacter* Player : Players)
    {
        Player->FootballerRecordId = SquadByShirt[Player->ShirtNumber];
    }
    return true;
}
