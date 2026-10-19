#include "SportsArcadeSeasonFactory.h"
#include "SportsArcadeFootballDatabase.h"
#include "SportsArcadeSeasonSave.h"

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
