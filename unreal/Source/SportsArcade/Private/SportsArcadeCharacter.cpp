#include "SportsArcadeCharacter.h"
#include "SportsArcadeBall.h"
#include "SportsArcadePlayerAttributes.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASportsArcadeCharacter::ASportsArcadeCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
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
}

void ASportsArcadeCharacter::MoveForward(float Value)
{
    if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        AddMovementInput(FVector::ForwardVector, Value);
    }
}

void ASportsArcadeCharacter::MoveRight(float Value)
{
    if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        AddMovementInput(FVector::RightVector, Value);
    }
}

void ASportsArcadeCharacter::BeginSprint()
{
    bSprintHeld = true;
}

void ASportsArcadeCharacter::EndSprint()
{
    bSprintHeld = false;
    GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
}

float ASportsArcadeCharacter::GetStaminaNormalized() const
{
    return FMath::Clamp(CurrentStamina / FMath::Max(MaxStamina, 1.f), 0.f, 1.f);
}

void ASportsArcadeCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (DeltaSeconds <= 0.f) return;

    const float Capacity = FMath::Max(MaxStamina, 1.f);
    CurrentStamina = FMath::Clamp(CurrentStamina, 0.f, Capacity);
    if (bSprintExhausted && CurrentStamina >= FMath::Min(StaminaToResumeSprint, Capacity))
    {
        bSprintExhausted = false;
    }
    const bool bMoving = GetVelocity().SizeSquared2D() > FMath::Square(10.f);
    const bool bSprinting = bSprintHeld && bMoving && !bSprintExhausted && CurrentStamina > 0.f;
    if (bSprinting)
    {
        CurrentStamina = FMath::Max(0.f, CurrentStamina - SprintDrainPerSecond * DeltaSeconds);
        if (CurrentStamina <= 0.f) bSprintExhausted = true;
    }
    else
    {
        CurrentStamina = FMath::Min(Capacity, CurrentStamina + StaminaRecoveryPerSecond * DeltaSeconds);
    }
    GetCharacterMovement()->MaxWalkSpeed = bSprinting && !bSprintExhausted ? SprintSpeed : JogSpeed;
}

void ASportsArcadeCharacter::PassBall()
{
    StrikeNearbyBall(PassImpulseKgCmPerSec);
}

void ASportsArcadeCharacter::ShootBall()
{
    StrikeNearbyBall(ShotImpulseKgCmPerSec);
}

void ASportsArcadeCharacter::StrikeNearbyBall(float ImpulseStrength)
{
    UWorld* World = GetWorld();
    if (!World || ImpulseStrength <= 0.f)
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
        // Starter ground pass/shot: no aim assist, contact animation, or spin yet.
        const FVector Direction = GetActorForwardVector().GetSafeNormal2D();
        NearestBall->Kick(Direction, ImpulseStrength);
    }
}
