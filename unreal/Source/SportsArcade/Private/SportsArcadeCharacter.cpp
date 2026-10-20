#include "SportsArcadeCharacter.h"
#include "SportsArcadeBall.h"
#include "SportsArcadePlayerAttributes.h"
#include "SportsArcadeGameMode.h"
#include "SportsArcadeTeamAIController.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASportsArcadeCharacter::ASportsArcadeCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    AIControllerClass = ASportsArcadeTeamAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    PlayerAttributes = CreateDefaultSubobject<USportsArcadePlayerAttributes>(TEXT("PlayerAttributes"));
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
    GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
    GetCharacterMovement()->BrakingDecelerationWalking = 2100.f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 800.f;
    CameraBoom->SetRelativeRotation(FRotator(-37.f, 0.f, 0.f));
    CameraBoom->bUsePawnControlRotation = false;
    CameraBoom->bDoCollisionTest = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
    FollowCamera->FieldOfView = 56.f;
}

void ASportsArcadeCharacter::BeginPlay()
{
    Super::BeginPlay();
    InitialKickoffTransform = GetActorTransform();
    const float StaminaFactor = PlayerAttributes ? USportsArcadePlayerAttributes::RatingFactor(PlayerAttributes->Stamina) : 1.f;
    CurrentStamina = FMath::Max(MaxStamina * StaminaFactor, 1.f);
}

void ASportsArcadeCharacter::ResetToKickoffPosition()
{
    SetActorTransform(InitialKickoffTransform, false, nullptr, ETeleportType::TeleportPhysics);
    GetCharacterMovement()->StopMovementImmediately();
    bSprintHeld = false;
    bJockeyHeld = false;
    bSprintExhausted = false;
    CurrentStamina = FMath::Max(MaxStamina * (PlayerAttributes ? USportsArcadePlayerAttributes::RatingFactor(PlayerAttributes->Stamina) : 1.f), 1.f);
    TouchCooldownRemaining = 0.f;
    StrikeCooldownRemaining = 0.f;
    TackleCooldownRemaining = 0.f;
}

void ASportsArcadeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ASportsArcadeCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ASportsArcadeCharacter::MoveRight);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &ASportsArcadeCharacter::BeginSprint);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &ASportsArcadeCharacter::EndSprint);
    PlayerInputComponent->BindAction(TEXT("Pass"), IE_Pressed, this, &ASportsArcadeCharacter::PassBall);
    PlayerInputComponent->BindAction(TEXT("Shoot"), IE_Pressed, this, &ASportsArcadeCharacter::ShootBall);
    PlayerInputComponent->BindAction(TEXT("Chip"), IE_Pressed, this, &ASportsArcadeCharacter::ChipBall);
    PlayerInputComponent->BindAction(TEXT("Tackle"), IE_Pressed, this, &ASportsArcadeCharacter::TackleBall);
    PlayerInputComponent->BindAction(TEXT("Jockey"), IE_Pressed, this, &ASportsArcadeCharacter::BeginJockey);
    PlayerInputComponent->BindAction(TEXT("Jockey"), IE_Released, this, &ASportsArcadeCharacter::EndJockey);
    PlayerInputComponent->BindAction(TEXT("SwitchPlayer"), IE_Pressed, this, &ASportsArcadeCharacter::SwitchPlayer);
    PlayerInputComponent->BindAction(TEXT("ResumeSecondHalf"), IE_Pressed, this, &ASportsArcadeCharacter::ResumeMatchAfterHalfTime);
    PlayerInputComponent->BindAction(TEXT("RestartMatch"), IE_Pressed, this, &ASportsArcadeCharacter::RestartExhibitionMatch);
    PlayerInputComponent->BindAction(TEXT("RestartBallOut"), IE_Pressed, this, &ASportsArcadeCharacter::RestartAfterBallOut);
    PlayerInputComponent->BindAction(TEXT("TakeManualRestart"), IE_Pressed, this, &ASportsArcadeCharacter::TakeStagedRestart);
    PlayerInputComponent->BindAction(TEXT("AwardRestartHome"), IE_Pressed, this, &ASportsArcadeCharacter::AwardRestartToHome);
    PlayerInputComponent->BindAction(TEXT("AwardRestartAway"), IE_Pressed, this, &ASportsArcadeCharacter::AwardRestartToAway);
    PlayerInputComponent->BindAction(TEXT("StageCupPenalty"), IE_Pressed, this, &ASportsArcadeCharacter::StageShootoutPenalty);
    PlayerInputComponent->BindAction(TEXT("KickCupPenalty"), IE_Pressed, this, &ASportsArcadeCharacter::KickShootoutPenalty);
    PlayerInputComponent->BindAction(TEXT("PenaltyGoalReview"), IE_Pressed, this, &ASportsArcadeCharacter::AwardPenaltyGoal);
    PlayerInputComponent->BindAction(TEXT("PenaltyMissReview"), IE_Pressed, this, &ASportsArcadeCharacter::AwardPenaltyMiss);
    PlayerInputComponent->BindAction(TEXT("ConfirmPenaltySuggestion"), IE_Pressed, this, &ASportsArcadeCharacter::ConfirmPenaltySuggestion);
}

void ASportsArcadeCharacter::MoveForward(float Value)
{
    const ASportsArcadeGameMode* Match = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this));
    if (IsValid(Match) && (Match->bHalfTime || Match->bMatchFinished || Match->bBallOutOfPlay)) return;
    if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        AddMovementInput(FVector::ForwardVector, Value);
    }
}

void ASportsArcadeCharacter::MoveRight(float Value)
{
    const ASportsArcadeGameMode* Match = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this));
    if (IsValid(Match) && (Match->bHalfTime || Match->bMatchFinished || Match->bBallOutOfPlay)) return;
    if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        AddMovementInput(FVector::RightVector, Value);
    }
}

void ASportsArcadeCharacter::BeginSprint()
{
    bSprintHeld = true;
}

void ASportsArcadeCharacter::BeginJockey()
{
    bJockeyHeld = true;
}

void ASportsArcadeCharacter::EndJockey()
{
    bJockeyHeld = false;
}

void ASportsArcadeCharacter::EndSprint()
{
    bSprintHeld = false;
    const float PaceFactor = PlayerAttributes ? USportsArcadePlayerAttributes::RatingFactor(PlayerAttributes->SprintSpeed) : 1.f;
    GetCharacterMovement()->MaxWalkSpeed = JogSpeed * PaceFactor;
}

float ASportsArcadeCharacter::GetStaminaNormalized() const
{
    const float Factor = PlayerAttributes ? USportsArcadePlayerAttributes::RatingFactor(PlayerAttributes->Stamina) : 1.f;
    return FMath::Clamp(CurrentStamina / FMath::Max(MaxStamina * Factor, 1.f), 0.f, 1.f);
}

void ASportsArcadeCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (DeltaSeconds <= 0.f) return;
    TouchCooldownRemaining = FMath::Max(0.f, TouchCooldownRemaining - DeltaSeconds);
    TackleCooldownRemaining = FMath::Max(0.f, TackleCooldownRemaining - DeltaSeconds);
    StrikeCooldownRemaining = FMath::Max(0.f, StrikeCooldownRemaining - DeltaSeconds);
    const ASportsArcadeGameMode* MatchMode = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this));
    const bool bPlayActive = !IsValid(MatchMode) || (!MatchMode->bHalfTime && !MatchMode->bMatchFinished && !MatchMode->bBallOutOfPlay);
    if (bPlayActive && IsPlayerControlled() && TouchCooldownRemaining <= 0.f &&
        GetVelocity().SizeSquared2D() > 400.f)
    {
        for (TActorIterator<ASportsArcadeBall> It(GetWorld()); It; ++It)
        {
            // Autonomous teammates use their own AI decision/cooldown logic.
            // A manual touch cannot automatically seize a ball at a nearer
            // teammate's feet during a crowded possession sequence.
            const FVector BallLocation = It->GetActorLocation();
            if (BallLocation.ContainsNaN()) continue;
            bool bFriendlyHasCloserControl = false;
            const float MyBallDistanceSq = FVector::DistSquared2D(GetActorLocation(), BallLocation);
            for (TActorIterator<ASportsArcadeCharacter> MateIt(GetWorld()); MateIt; ++MateIt)
            {
                const ASportsArcadeCharacter* Mate = *MateIt;
                if (Mate == this || Mate->TeamIndex != TeamIndex ||
                    Mate->GetActorLocation().ContainsNaN()) continue;
                const FVector MateToBall = BallLocation - Mate->GetActorLocation();
                if (FMath::Abs(MateToBall.Z) <= 115.f &&
                    MateToBall.SizeSquared2D() < FMath::Square(115.f) &&
                    MateToBall.SizeSquared2D() < MyBallDistanceSq)
                {
                    bFriendlyHasCloserControl = true;
                    break;
                }
            }
            if (bFriendlyHasCloserControl) continue;
            const FVector ToBall = BallLocation - GetActorLocation();
            if (FVector::DotProduct(GetActorForwardVector(), ToBall.GetSafeNormal2D()) > 0.35f &&
                It->ApplyControlledTouch(this, GetVelocity().GetSafeNormal2D() *
                    (240.f * (PlayerAttributes ? USportsArcadePlayerAttributes::RatingFactor(PlayerAttributes->BallControl) : 1.f)), DribbleTouchRangeCm))
            {
                TouchCooldownRemaining = FMath::Max(DribbleTouchInterval, 0.05f);
                break;
            }
        }
    }

    const float StaminaFactor = PlayerAttributes ? USportsArcadePlayerAttributes::RatingFactor(PlayerAttributes->Stamina) : 1.f;
    const float Capacity = FMath::Max(MaxStamina * StaminaFactor, 1.f);
    CurrentStamina = FMath::Clamp(CurrentStamina, 0.f, Capacity);
    if (bSprintExhausted && CurrentStamina >= FMath::Min(StaminaToResumeSprint, Capacity))
    {
        bSprintExhausted = false;
    }
    const bool bMoving = GetVelocity().SizeSquared2D() > FMath::Square(10.f);
    const bool bSprinting = bPlayActive && !bJockeyHeld && bSprintHeld && bMoving && !bSprintExhausted && CurrentStamina > 0.f;
    if (bSprinting)
    {
        CurrentStamina = FMath::Max(0.f, CurrentStamina - SprintDrainPerSecond * DeltaSeconds);
        if (CurrentStamina <= 0.f) bSprintExhausted = true;
    }
    else
    {
        CurrentStamina = FMath::Min(Capacity, CurrentStamina + StaminaRecoveryPerSecond * DeltaSeconds);
    }
    const float PaceFactor = PlayerAttributes ? USportsArcadePlayerAttributes::RatingFactor(PlayerAttributes->SprintSpeed) : 1.f;
    // Low stamina progressively reduces top speed and acceleration instead
    // of switching between unrealistic full-speed and exhausted movement.
    // Keep the minimum high enough to permit defensive recovery and jockeying.
    const float StaminaFraction = CurrentStamina / Capacity;
    const float FatigueSpeedFactor = FMath::Lerp(0.72f, 1.f,
        FMath::Clamp(StaminaFraction / 0.4f, 0.f, 1.f));
    GetCharacterMovement()->MaxWalkSpeed = (bJockeyHeld && bPlayActive ? JockeySpeed :
        (bSprinting && !bSprintExhausted ? SprintSpeed : JogSpeed)) *
        PaceFactor * FatigueSpeedFactor;
    const float AccelerationFactor = PlayerAttributes ? USportsArcadePlayerAttributes::RatingFactor(PlayerAttributes->Acceleration) : 1.f;
    GetCharacterMovement()->MaxAcceleration = 1600.f * AccelerationFactor * FatigueSpeedFactor;
    // Defensive jockeying uses shorter, more controllable stops than open running.
    GetCharacterMovement()->BrakingDecelerationWalking = bJockeyHeld && bPlayActive ? 2800.f : 2100.f;
}

void ASportsArcadeCharacter::PassBall()
{
    const float Factor = PlayerAttributes ? USportsArcadePlayerAttributes::RatingFactor(PlayerAttributes->ShortPassing) : 1.f;
    StrikeNearbyBall(PassImpulseKgCmPerSec * Factor, false);
}

void ASportsArcadeCharacter::ShootBall()
{
    const float Factor = PlayerAttributes ? USportsArcadePlayerAttributes::RatingFactor(PlayerAttributes->Finishing) : 1.f;
    StrikeNearbyBall(ShotImpulseKgCmPerSec * Factor, true);
}

void ASportsArcadeCharacter::ChipBall()
{
    const float Factor = PlayerAttributes ? USportsArcadePlayerAttributes::RatingFactor(PlayerAttributes->Finishing) : 1.f;
    StrikeNearbyBall(ShotImpulseKgCmPerSec * Factor * ChipPowerFactor, true, ChipLiftRatio);
}

void ASportsArcadeCharacter::StrikeNearbyBall(float ImpulseStrength, bool bIsShot, float LiftRatio)
{
    UWorld* World = GetWorld();
    ASportsArcadeGameMode* MatchMode = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this));
    if (!World || ImpulseStrength <= 0.f || StrikeCooldownRemaining > 0.f ||
        (IsValid(MatchMode) && (MatchMode->bHalfTime || MatchMode->bMatchFinished || MatchMode->bBallOutOfPlay)))
    {
        return;
    }

    ASportsArcadeBall* NearestBall = nullptr;
    float NearestDistanceSquared = FMath::Square(BallControlReachCm);
    const FVector PlayerLocation = GetActorLocation();
    for (TActorIterator<ASportsArcadeBall> It(World); It; ++It)
    {
        const float DistanceSquared = FVector::DistSquared(PlayerLocation, It->GetActorLocation());
        if (DistanceSquared <= NearestDistanceSquared)
        {
            NearestDistanceSquared = DistanceSquared;
            NearestBall = *It;
        }
    }

    if (IsValid(NearestBall))
    {
        const FVector ToBall = NearestBall->GetActorLocation() - PlayerLocation;
        // Aim with the current movement input when available; standing players use facing.
        const FVector InputAim = GetLastMovementInputVector().GetSafeNormal2D();
        const FVector Forward = InputAim.IsNearlyZero() ? GetActorForwardVector().GetSafeNormal2D() : InputAim;
        if (FMath::Abs(ToBall.Z) <= 120.f && FVector::DotProduct(GetActorForwardVector().GetSafeNormal2D(), ToBall.GetSafeNormal2D()) > 0.15f)
        {
            // A shot gains moderate lift; a ground pass stays flat.
            const float AppliedLift = LiftRatio > 0.f ? LiftRatio : ShotLiftRatio;
            const FVector StrikeDirection = bIsShot ? (Forward + FVector(0.f, 0.f, AppliedLift)).GetSafeNormal() : Forward;
            if (NearestBall->KickByPlayer(this, StrikeDirection, ImpulseStrength, BallControlReachCm))
            {
                if (bIsShot && IsValid(MatchMode)) MatchMode->RecordShotAttempt(TeamIndex);
                StrikeCooldownRemaining = FMath::Max(StrikeCooldownSeconds, 0.08f);
                TouchCooldownRemaining = FMath::Max(TouchCooldownRemaining, 0.2f);
            }
        }
    }
}

void ASportsArcadeCharacter::SwitchPlayer()
{
    if (ASportsArcadeGameMode* Mode = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        Mode->SwitchToNearestTeammate(TeamIndex);
    }
}

void ASportsArcadeCharacter::ResumeMatchAfterHalfTime()
{
    if (ASportsArcadeGameMode* Mode = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        Mode->ResumeSecondHalf();
    }
}

void ASportsArcadeCharacter::RestartExhibitionMatch()
{
    if (ASportsArcadeGameMode* Mode = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        Mode->ResetMatch();
    }
}

void ASportsArcadeCharacter::RestartAfterBallOut()
{
    if (ASportsArcadeGameMode* Mode = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        Mode->ResumeAfterBallOut();
    }
}

void ASportsArcadeCharacter::TakeStagedRestart()
{
    if (ASportsArcadeGameMode* Mode = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        if (Mode->CanTakeManualRestart() && IsValid(Mode->PendingRestartTaker))
        {
            Mode->TakeManualRestart(Mode->PendingRestartTaker->GetActorForwardVector());
        }
    }
}

void ASportsArcadeCharacter::AwardRestartToHome()
{
    if (ASportsArcadeGameMode* Mode = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        Mode->AwardUndeterminedRestart(0);
    }
}

void ASportsArcadeCharacter::AwardRestartToAway()
{
    if (ASportsArcadeGameMode* Mode = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        Mode->AwardUndeterminedRestart(1);
    }
}

void ASportsArcadeCharacter::StageShootoutPenalty()
{
    if (ASportsArcadeGameMode* Mode = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        if (Mode->CanStageCupPenaltyKick()) Mode->StageCupPenaltyKick();
    }
}

void ASportsArcadeCharacter::KickShootoutPenalty()
{
    if (ASportsArcadeGameMode* Mode = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        if (ASportsArcadeCharacter* Taker = Mode->GetStagedCupPenaltyTaker())
        {
            Mode->KickStagedCupPenalty(Taker->GetActorForwardVector());
        }
    }
}

void ASportsArcadeCharacter::AwardPenaltyGoal()
{
    if (ASportsArcadeGameMode* Mode = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        Mode->ResolvePhysicalCupPenalty(true);
    }
}

void ASportsArcadeCharacter::AwardPenaltyMiss()
{
    if (ASportsArcadeGameMode* Mode = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        Mode->ResolvePhysicalCupPenalty(false);
    }
}

void ASportsArcadeCharacter::ConfirmPenaltySuggestion()
{
    if (ASportsArcadeGameMode* Mode = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this)))
        Mode->ConfirmSuggestedCupPenaltyOutcome();
}

void ASportsArcadeCharacter::TackleBall()
{
    const ASportsArcadeGameMode* MatchMode = Cast<ASportsArcadeGameMode>(UGameplayStatics::GetGameMode(this));
    if (!GetWorld() || TackleCooldownRemaining > 0.f ||
        (IsValid(MatchMode) && (MatchMode->bHalfTime || MatchMode->bMatchFinished || MatchMode->bBallOutOfPlay))) return;

    // A missed standing tackle is still a committed defensive action; repeated
    // presses must not bypass the recovery window while the ball approaches.
    TackleCooldownRemaining = FMath::Max(TackleCooldownSeconds, 0.1f);
    const FVector DefenderLocation = GetActorLocation();
    const FVector Forward = GetActorForwardVector().GetSafeNormal2D();
    if (DefenderLocation.ContainsNaN() || Forward.IsNearlyZero()) return;

    ASportsArcadeBall* CandidateBall = nullptr;
    float ClosestDistanceSq = FMath::Square(150.f);
    for (TActorIterator<ASportsArcadeBall> It(GetWorld()); It; ++It)
    {
        const FVector BallLocation = It->GetActorLocation();
        if (BallLocation.ContainsNaN()) continue;
        const FVector Direction = BallLocation - DefenderLocation;
        const float DistanceSq = Direction.SizeSquared2D();
        if (DistanceSq >= ClosestDistanceSq || FMath::Abs(Direction.Z) >= 110.f ||
            FVector::DotProduct(Forward, Direction.GetSafeNormal2D()) <= 0.25f) continue;

        ASportsArcadeCharacter* LastTouch = It->GetLastTouchPlayer();
        if (IsValid(LastTouch) && LastTouch->TeamIndex == TeamIndex) continue;

        // Last touch is not possession: avoid knocking the ball away from a
        // nearby teammate even after an opponent's older recorded touch.
        bool bTeammateControllingBall = false;
        for (TActorIterator<ASportsArcadeCharacter> PlayerIt(GetWorld()); PlayerIt; ++PlayerIt)
        {
            ASportsArcadeCharacter* Other = *PlayerIt;
            if (Other == this || Other->TeamIndex != TeamIndex ||
                Other->GetActorLocation().ContainsNaN()) continue;
            const FVector OtherToBall = BallLocation - Other->GetActorLocation();
            if (OtherToBall.SizeSquared2D() < FMath::Square(115.f) &&
                FMath::Abs(OtherToBall.Z) < 115.f)
            {
                bTeammateControllingBall = true;
                break;
            }
        }
        if (bTeammateControllingBall) continue;

        CandidateBall = *It;
        ClosestDistanceSq = DistanceSq;
    }

    if (IsValid(CandidateBall))
    {
        CandidateBall->KickByPlayer(this, Forward,
            850.f * (PlayerAttributes ? USportsArcadePlayerAttributes::RatingFactor(PlayerAttributes->Tackling) : 1.f),
            175.f);
    }
}
