#include "SportsArcadePenaltyShootout.h"

void USportsArcadePenaltyShootout::ResetShootout()
{
    HomeAttempts = 0;
    AwayAttempts = 0;
    HomeScored = 0;
    AwayScored = 0;
    WinningTeam = INDEX_NONE;
}

int32 USportsArcadePenaltyShootout::GetNextKickingTeam() const
{
    if (WinningTeam != INDEX_NONE || HomeAttempts < 0 || AwayAttempts < 0 ||
        HomeAttempts < AwayAttempts || HomeAttempts > AwayAttempts + 1 ||
        HomeScored < 0 || AwayScored < 0 ||
        HomeScored > HomeAttempts || AwayScored > AwayAttempts) return INDEX_NONE;
    return HomeAttempts == AwayAttempts ? 0 : 1;
}

bool USportsArcadePenaltyShootout::RecordAttempt(int32 TeamIndex, bool bScored)
{
    if (TeamIndex != GetNextKickingTeam()) return false;
    if (TeamIndex == 0)
    {
        ++HomeAttempts;
        if (bScored) ++HomeScored;
    }
    else
    {
        ++AwayAttempts;
        if (bScored) ++AwayScored;
    }

    // An insurmountable lead within the initial five kicks ends immediately.
    if (HomeAttempts <= 5 && AwayAttempts <= 5)
    {
        if (HomeScored > AwayScored + (5 - AwayAttempts)) WinningTeam = 0;
        else if (AwayScored > HomeScored + (5 - HomeAttempts)) WinningTeam = 1;
    }
    // Beyond five kicks, sudden death settles only after BOTH sides kick.
    if (WinningTeam == INDEX_NONE && HomeAttempts >= 5 &&
        HomeAttempts == AwayAttempts && HomeScored != AwayScored)
    {
        WinningTeam = HomeScored > AwayScored ? 0 : 1;
    }
    return true;
}
