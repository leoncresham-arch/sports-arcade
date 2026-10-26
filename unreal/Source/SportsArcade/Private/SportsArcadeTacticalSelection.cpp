#include "SportsArcadeTacticalSelection.h"

bool FSportsArcadeTacticalSelection::Before(
    const FSportsArcadePressCandidate& Candidate,
    const FSportsArcadePressCandidate& Current)
{
    return Candidate.DistanceSquared < Current.DistanceSquared - 1.f ||
        (FMath::Abs(Candidate.DistanceSquared - Current.DistanceSquared) <= 1.f &&
            Candidate.Slot < Current.Slot);
}

FSportsArcadePressPair FSportsArcadeTacticalSelection::SelectPressPair(
    const TArray<FSportsArcadePressCandidate>& Eligible)
{
    FSportsArcadePressCandidate First, Second;
    FSportsArcadePressPair Result;
    for (const FSportsArcadePressCandidate& Candidate : Eligible)
    {
        if (Candidate.Slot < 1 || Candidate.Slot > 10 ||
            !FMath::IsFinite(Candidate.DistanceSquared) ||
            Candidate.DistanceSquared < 0.f) continue;
        // Keep slot identity unique even if a malformed caller supplies
        // repeated lineup records. The better reading wins deterministically.
        if (Candidate.Slot == First.Slot)
        {
            if (Before(Candidate, First)) First = Candidate;
            continue;
        }
        if (Candidate.Slot == Second.Slot)
        {
            if (Before(Candidate, Second)) Second = Candidate;
            continue;
        }
        if (Before(Candidate, First))
        {
            Second = First;
            First = Candidate;
        }
        else if (Before(Candidate, Second))
        {
            Second = Candidate;
        }
    }
    Result.PrimarySlot = First.Slot;
    Result.CoverSlot = Second.Slot;
    return Result;
}
