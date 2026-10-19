#include "SportsArcadeSeasonFactory.h"
#include "SportsArcadeFootballDatabase.h"
#include "SportsArcadeSeasonSave.h"
#include "SportsArcadeCupSave.h"

USportsArcadeSeasonSave* USportsArcadeSeasonFactory::CreateLeagueSeason(
    const UObject* WorldContextObject,
    const USportsArcadeFootballDatabase* Catalog,
    FName CompetitionId,
    bool bHomeAndAway)
{
    if (!IsValid(WorldContextObject) || !IsValid(Catalog) || CompetitionId.IsNone())
        return nullptr;

    TArray<FString> Errors;
    if (!Catalog->ValidateDatabase(Errors))
        return nullptr;

    for (const FSportsArcadeCompetitionRecord& Competition : Catalog->Competitions)
    {
        if (Competition.Id != CompetitionId) continue;
        if (Competition.Format != ESportsArcadeCompetitionFormat::League ||
            Competition.ParticipantClubIds.Num() < 2)
            return nullptr;

        USportsArcadeSeasonSave* Season = NewObject<USportsArcadeSeasonSave>(
            GetTransientPackage(), USportsArcadeSeasonSave::StaticClass());
        if (!IsValid(Season) ||
            !Season->StartSeason(Competition.Id, Competition.ParticipantClubIds, bHomeAndAway))
            return nullptr;
        return Season;
    }
    return nullptr;
}

USportsArcadeCupSave* USportsArcadeSeasonFactory::CreateKnockoutCup(
    const UObject* WorldContextObject,
    const USportsArcadeFootballDatabase* Catalog,
    FName CompetitionId)
{
    if (!IsValid(WorldContextObject) || !IsValid(Catalog) || CompetitionId.IsNone())
        return nullptr;
    TArray<FString> Errors;
    if (!Catalog->ValidateDatabase(Errors)) return nullptr;
    for (const FSportsArcadeCompetitionRecord& Competition : Catalog->Competitions)
    {
        if (Competition.Id != CompetitionId) continue;
        if (Competition.Format != ESportsArcadeCompetitionFormat::Knockout)
            return nullptr;
        USportsArcadeCupSave* Cup = NewObject<USportsArcadeCupSave>(
            GetTransientPackage(), USportsArcadeCupSave::StaticClass());
        return IsValid(Cup) &&
            Cup->StartCup(Competition.Id, Competition.ParticipantClubIds) ? Cup : nullptr;
    }
    return nullptr;
}
