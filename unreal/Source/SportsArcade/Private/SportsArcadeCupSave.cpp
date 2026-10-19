#include "SportsArcadeCupSave.h"

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
