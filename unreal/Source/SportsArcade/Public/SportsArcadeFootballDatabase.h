#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SportsArcadeFootballDatabase.generated.h"

/** Original, authored football data; no bundled third-party rosters or licensed art. */
UENUM(BlueprintType)
enum class ESportsArcadeCompetitionFormat : uint8
{
    League,
    Knockout,
    GroupsAndKnockout
};

UENUM(BlueprintType)
enum class ESportsArcadeSquadCategory : uint8
{
    Men,
    Women
};

USTRUCT(BlueprintType)
struct FSportsArcadeNationRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Id;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ConfederationId;
};

USTRUCT(BlueprintType)
struct FSportsArcadeClubRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Id;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName NationId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ESportsArcadeSquadCategory Category = ESportsArcadeSquadCategory::Men;
};

USTRUCT(BlueprintType)
struct FSportsArcadeFootballerRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Id;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName NationId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ClubId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="99"))
    int32 ShirtNumber = 1;
};

USTRUCT(BlueprintType)
struct FSportsArcadeCompetitionRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Id;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName NationId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ESportsArcadeSquadCategory Category = ESportsArcadeSquadCategory::Men;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ESportsArcadeCompetitionFormat Format = ESportsArcadeCompetitionFormat::League;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FName> ParticipantClubIds;
};

/** Extensible data asset for game-mode selection, offline seasons and team rosters. */
UCLASS(BlueprintType)
class SPORTSARCADE_API USportsArcadeFootballDatabase : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Football Database")
    TArray<FSportsArcadeNationRecord> Nations;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Football Database")
    TArray<FSportsArcadeClubRecord> Clubs;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Football Database")
    TArray<FSportsArcadeFootballerRecord> Footballers;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Football Database")
    TArray<FSportsArcadeCompetitionRecord> Competitions;

    /** Refuses duplicate/empty ids, missing references and mismatched squad categories. */
    UFUNCTION(BlueprintCallable, Category="Football Database")
    bool ValidateDatabase(TArray<FString>& Errors) const;

    UFUNCTION(BlueprintCallable, Category="Football Database")
    bool FindClubById(FName ClubId, FSportsArcadeClubRecord& OutClub) const;
};
