#pragma once
#include "CoreMinimal.h"
#include "SportsArcadeTacticalSelection.generated.h"

/** Candidate independent of a live Unreal world; invalid roles excluded. */
USTRUCT()
struct FSportsArcadePressCandidate
{
    GENERATED_BODY()
    int32 Slot = INDEX_NONE;
    float DistanceSquared = TNumericLimits<float>::Max();

    FSportsArcadePressCandidate() = default;
    FSportsArcadePressCandidate(int32 InSlot, float InDistanceSquared)
        : Slot(InSlot), DistanceSquared(InDistanceSquared) {}
};

USTRUCT()
struct FSportsArcadePressPair
{
    GENERATED_BODY()
    int32 PrimarySlot = INDEX_NONE;
    int32 CoverSlot = INDEX_NONE;
};

struct FSportsArcadeTacticalSelection
{
    static bool Before(const FSportsArcadePressCandidate& Candidate,
        const FSportsArcadePressCandidate& Current);
    static FSportsArcadePressPair SelectPressPair(
        const TArray<FSportsArcadePressCandidate>& Eligible);
};
