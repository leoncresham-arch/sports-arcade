#include "SportsArcadeCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASportsArcadeCharacter::ASportsArcadeCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
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
