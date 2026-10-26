#pragma once

#include "CoreMinimal.h"

/** Lightweight, actor-independent defensive assignments. Inputs are already
 * filtered for valid autonomous outfield players by the caller. The helper
 * can be exercised in Unreal automation without spawning a world or pawn. */
struct FSportsArcadePressCandidate
{
    int32 Slot = INDEX_NONE;
    float DistanceSquared = TNumericLimits<float>::Max();
};

struct FSportsArcadePressPair
{
    int32 PrimarySlot = INDEX_NONE;
    int32 CoverSlot = INDEX_NONE;
};

struct FSportsArcadeTacticalSelection
{
    static bool Before(const FSportsArcadePressCandidate& Candidate,
        const FSportsArcadePressCandidate& Current)
    {
        return Candidate.DistanceSquared < Current.DistanceSquared - 1.f ||
            (FMath::Abs(Candidate.DistanceSquared - Current.DistanceSquared) <= 1.f &&
                Candidate.Slot < Current.Slot);
    }

    static FSportsArcadePressPair SelectPressPair(
        const TArray<FSportsArcadePressCandidate>& Eligible)
    {
        FSportsArcadePressCandidate First, Second;
        FSportsArcadePressPair Result;
        for (const FSportsArcadePressCandidate& Candidate : Eligible)
        {
            if (Candidate.Slot < 1 || Candidate.Slot > 10 ||
                !FMath::IsFinite(Candidate.DistanceSquared) ||
                Candidate.DistanceSquared < 0.f) continue;
            if (Before(Candidate, First))
            {
                Second = First;
                First = Candidate;
            }
            else if (Candidate.Slot != First.Slot && Before(Candidate, Second))
            {
                Second = Candidate;
            }
        }
        Result.PrimarySlot = First.Slot;
        Result.CoverSlot = Second.Slot;
        return Result;
    }
};
