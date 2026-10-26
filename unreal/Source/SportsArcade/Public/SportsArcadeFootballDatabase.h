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

    // Optional real-world-inspired identity and tactical customisation.
    // Empty values retain the generic game baseline.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName HomeStadiumId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName PreferredFormationId;
};

USTRUCT(BlueprintType)
struct FSportsArcadeStadiumRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Id;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString DisplayName;

    // Presentation dimensions are authored; match-pitch defaults remain safe.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1000"))
    int32 Capacity = 30000;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="90.0", ClampMax="120.0"))
    float PitchLengthMetres = 105.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="45.0", ClampMax="90.0"))
    float PitchWidthMetres = 68.f;
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

    // Individual match-performance traits. The neutral 65 values deliberately
    // avoid misrepresenting real-world player statistics until authored.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="99"))
    int32 Acceleration = 65;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="99"))
    int32 Agility = 65;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="99"))
    int32 SprintSpeed = 65;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="99"))
    int32 Stamina = 65;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="99"))
    int32 ShortPassing = 65;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="99"))
    int32 Finishing = 65;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="99"))
    int32 BallControl = 65;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1", ClampMax="99"))
    int32 Tackling = 65;
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
    TArray<FSportsArcadeStadiumRecord> Stadiums;

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
