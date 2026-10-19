#include "SportsArcadeFootballDatabase.h"

bool USportsArcadeFootballDatabase::ValidateDatabase(TArray<FString>& Errors) const
{
    Errors.Reset();
    TSet<FName> NationIds;
    TMap<FName, ESportsArcadeSquadCategory> ClubCategories;
    TMap<FName, FName> ClubNations;
    TSet<FName> FootballerIds;
    TSet<FName> CompetitionIds;

    for (const FSportsArcadeNationRecord& Nation : Nations)
    {
        if (Nation.Id.IsNone() || Nation.DisplayName.TrimStartAndEnd().IsEmpty() || NationIds.Contains(Nation.Id))
            Errors.Add(FString::Printf(TEXT("Invalid or duplicate nation: %s"), *Nation.Id.ToString()));
        else NationIds.Add(Nation.Id);
    }
    for (const FSportsArcadeClubRecord& Club : Clubs)
    {
        if (Club.Id.IsNone() || Club.DisplayName.TrimStartAndEnd().IsEmpty() || ClubCategories.Contains(Club.Id))
            Errors.Add(FString::Printf(TEXT("Invalid or duplicate club: %s"), *Club.Id.ToString()));
        else
        {
            ClubCategories.Add(Club.Id, Club.Category);
            ClubNations.Add(Club.Id, Club.NationId);
        }
        if (!NationIds.Contains(Club.NationId))
            Errors.Add(FString::Printf(TEXT("Club %s references missing nation"), *Club.Id.ToString()));
    }
    for (const FSportsArcadeFootballerRecord& Player : Footballers)
    {
        if (Player.Id.IsNone() || Player.DisplayName.TrimStartAndEnd().IsEmpty() || FootballerIds.Contains(Player.Id))
            Errors.Add(FString::Printf(TEXT("Invalid or duplicate footballer: %s"), *Player.Id.ToString()));
        else FootballerIds.Add(Player.Id);
        if (!NationIds.Contains(Player.NationId))
            Errors.Add(FString::Printf(TEXT("Footballer %s references missing nationality"), *Player.Id.ToString()));
        if (!ClubCategories.Contains(Player.ClubId))
            Errors.Add(FString::Printf(TEXT("Footballer %s references missing club"), *Player.Id.ToString()));
        if (Player.ShirtNumber < 1 || Player.ShirtNumber > 99)
            Errors.Add(FString::Printf(TEXT("Footballer %s has invalid shirt number"), *Player.Id.ToString()));
    }
    for (const FSportsArcadeCompetitionRecord& Competition : Competitions)
    {
        if (Competition.Id.IsNone() || Competition.DisplayName.TrimStartAndEnd().IsEmpty() || CompetitionIds.Contains(Competition.Id))
            Errors.Add(FString::Printf(TEXT("Invalid or duplicate competition: %s"), *Competition.Id.ToString()));
        else CompetitionIds.Add(Competition.Id);
        if (!Competition.NationId.IsNone() && !NationIds.Contains(Competition.NationId))
            Errors.Add(FString::Printf(TEXT("Competition %s references missing nation"), *Competition.Id.ToString()));
        TSet<FName> Participants;
        for (const FName ClubId : Competition.ParticipantClubIds)
        {
            if (!ClubCategories.Contains(ClubId) || Participants.Contains(ClubId))
                Errors.Add(FString::Printf(TEXT("Competition %s has missing or repeated participant %s"), *Competition.Id.ToString(), *ClubId.ToString()));
            else if (ClubCategories[ClubId] != Competition.Category)
                Errors.Add(FString::Printf(TEXT("Competition %s has mismatched squad category"), *Competition.Id.ToString()));
            else Participants.Add(ClubId);
        }
    }
    return Errors.IsEmpty();
}

bool USportsArcadeFootballDatabase::FindClubById(FName ClubId, FSportsArcadeClubRecord& OutClub) const
{
    if (ClubId.IsNone()) return false;
    for (const FSportsArcadeClubRecord& Club : Clubs)
    {
        if (Club.Id == ClubId)
        {
            OutClub = Club;
            return true;
        }
    }
    return false;
}
