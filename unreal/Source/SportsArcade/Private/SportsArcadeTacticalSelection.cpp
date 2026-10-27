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
    // Aggregate each physical lineup slot before ranking. This prevents a
    // better duplicate reading from accidentally displacing the cover slot.
    TMap<int32, float> BestDistanceBySlot;
    for (const FSportsArcadePressCandidate& Candidate : Eligible)
    {
        if (Candidate.Slot < 1 || Candidate.Slot > 10 ||
            !FMath::IsFinite(Candidate.DistanceSquared) ||
            Candidate.DistanceSquared < 0.f) continue;
        float* Existing = BestDistanceBySlot.Find(Candidate.Slot);
        if (!Existing || Candidate.DistanceSquared < *Existing)
            BestDistanceBySlot.Add(Candidate.Slot, Candidate.DistanceSquared);
    }
    FSportsArcadePressCandidate First, Second;
    FSportsArcadePressPair Result;
    for (const TPair<int32, float>& Entry : BestDistanceBySlot)
    {
        const FSportsArcadePressCandidate Candidate(Entry.Key, Entry.Value);
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
