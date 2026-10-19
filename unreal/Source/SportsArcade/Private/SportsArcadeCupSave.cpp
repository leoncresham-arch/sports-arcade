#include "SportsArcadeCupSave.h"
#include "Kismet/GameplayStatics.h"

bool USportsArcadeCupSave::StartCup(FName InCompetitionId, const TArray<FName>& ClubIds)
{
    if (InCompetitionId.IsNone()) return false;
    TArray<FSportsArcadeCupFixture> OpeningRound;
    if (!USportsArcadeCupFixtures::GenerateOpeningRound(ClubIds, OpeningRound))
        return false;
    CompetitionId = InCompetitionId;
    CurrentRoundFixtures = MoveTemp(OpeningRound);
    CurrentRoundWinners.Reset();
    ChampionId = NAME_None;
    return true;
}

bool USportsArcadeCupSave::RecordWinner(FName WinningClubId)
{
    if (IsCupComplete() || WinningClubId.IsNone() ||
        !CurrentRoundFixtures.IsValidIndex(CurrentRoundWinners.Num()))
        return false;
    const FSportsArcadeCupFixture& Fixture = CurrentRoundFixtures[CurrentRoundWinners.Num()];
    if (WinningClubId != Fixture.HomeClubId && WinningClubId != Fixture.AwayClubId)
        return false;
    CurrentRoundWinners.Add(WinningClubId);
    if (CurrentRoundWinners.Num() == 1 && CurrentRoundFixtures.Num() == 1)
        ChampionId = WinningClubId;
    return true;
}

bool USportsArcadeCupSave::AdvanceRound()
{
    if (IsCupComplete() || CurrentRoundFixtures.Num() < 2 ||
        CurrentRoundWinners.Num() != CurrentRoundFixtures.Num())
        return false;
    TArray<FSportsArcadeCupFixture> NextFixtures;
    if (!USportsArcadeCupFixtures::GenerateNextRound(
        CurrentRoundFixtures, CurrentRoundWinners, NextFixtures))
        return false;
    CurrentRoundFixtures = MoveTemp(NextFixtures);
    CurrentRoundWinners.Reset();
    return true;
}

bool USportsArcadeCupSave::IsCupComplete() const
{
    return !ChampionId.IsNone();
}

bool USportsArcadeCupSave::SaveCupToSlot(const FString& SlotName, int32 UserIndex) const
{
    if (SlotName.IsEmpty() || UserIndex < 0 || !ValidateProgress()) return false;
    return UGameplayStatics::SaveGameToSlot(const_cast<USportsArcadeCupSave*>(this), SlotName, UserIndex);
}

USportsArcadeCupSave* USportsArcadeCupSave::LoadCupFromSlot(
    const UObject* WorldContextObject, const FString& SlotName, int32 UserIndex)
{
    if (!IsValid(WorldContextObject) || SlotName.IsEmpty() || UserIndex < 0)
        return nullptr;
    USportsArcadeCupSave* Cup = Cast<USportsArcadeCupSave>(
        UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
    if (!IsValid(Cup) || !Cup->ValidateProgress()) return nullptr;
    return Cup;
}

bool USportsArcadeCupSave::ValidateProgress() const
{
    if (CompetitionId.IsNone() || CurrentRoundFixtures.IsEmpty() ||
        CurrentRoundWinners.Num() > CurrentRoundFixtures.Num()) return false;
    const int32 Round = CurrentRoundFixtures[0].Round;
    if (Round < 1) return false;
    TSet<FName> SeenTeams;
    for (const FSportsArcadeCupFixture& Fixture : CurrentRoundFixtures)
    {
        if (Fixture.Round != Round || Fixture.HomeClubId.IsNone() ||
            Fixture.AwayClubId.IsNone() || Fixture.HomeClubId == Fixture.AwayClubId ||
            SeenTeams.Contains(Fixture.HomeClubId) ||
            SeenTeams.Contains(Fixture.AwayClubId)) return false;
        SeenTeams.Add(Fixture.HomeClubId);
        SeenTeams.Add(Fixture.AwayClubId);
    }
    TSet<FName> WinnersSeen;
    for (int32 Index = 0; Index < CurrentRoundWinners.Num(); ++Index)
    {
        const FName Winner = CurrentRoundWinners[Index];
        const FSportsArcadeCupFixture& Fixture = CurrentRoundFixtures[Index];
        if (WinnersSeen.Contains(Winner) ||
            (Winner != Fixture.HomeClubId && Winner != Fixture.AwayClubId)) return false;
        WinnersSeen.Add(Winner);
    }
    if (!ChampionId.IsNone() &&
        (CurrentRoundFixtures.Num() != 1 || CurrentRoundWinners.Num() != 1 ||
         ChampionId != CurrentRoundWinners[0])) return false;
    return true;
}
