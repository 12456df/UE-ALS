#include "MyALSCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MyALSCharacterMovementComponent.h"

AMyALSCharacter::AMyALSCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UMyALSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 350.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
}

void AMyALSCharacter::ApplyMoveInput(const FVector2D& Axis)
{
    if (UMyALSCharacterMovementComponent* Movement = Cast<UMyALSCharacterMovementComponent>(GetCharacterMovement()))
    {
        Movement->SetMoveInput(Axis);
    }
    if (!Controller)
    {
        return;
    }

    const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
    AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Axis.Y);
    AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Axis.X);
}

void AMyALSCharacter::ClearMoveInput()
{
    if (UMyALSCharacterMovementComponent* Movement = Cast<UMyALSCharacterMovementComponent>(GetCharacterMovement()))
    {
        Movement->SetMoveInput(FVector2D::ZeroVector);
    }
}

void AMyALSCharacter::ApplyLookInput(const FVector2D& Axis)
{
    AddControllerYawInput(Axis.X);
    AddControllerPitchInput(-Axis.Y);
}

void AMyALSCharacter::StartJumpInput()
{
    // 蹲伏时空格只请求起身；这一按不会继续触发跳跃。
    if (bIsCrouched || GetCharacterMovement()->bWantsToCrouch)
    {
        UnCrouch();
        return;
    }
    Jump();
}

void AMyALSCharacter::StopJumpInput()
{
    StopJumping();
}

void AMyALSCharacter::ToggleCrouchInput()
{
    UMyALSCharacterMovementComponent* Movement = Cast<UMyALSCharacterMovementComponent>(GetCharacterMovement());
    if (!Movement)
    {
        return;
    }

    if (Movement->bWantsToCrouch || bIsCrouched)
    {
        UnCrouch();
    }
    else
    {
        Crouch();
    }
}

void AMyALSCharacter::ToggleWalkInput()
{
    if (UMyALSCharacterMovementComponent* Movement = Cast<UMyALSCharacterMovementComponent>(GetCharacterMovement()))
    {
        Movement->ToggleWalk();
    }
}

void AMyALSCharacter::StartSprintInput()
{
    if (UMyALSCharacterMovementComponent* Movement = Cast<UMyALSCharacterMovementComponent>(GetCharacterMovement()))
    {
        Movement->SetSprintHeld(true);
    }
}

void AMyALSCharacter::StopSprintInput()
{
    if (UMyALSCharacterMovementComponent* Movement = Cast<UMyALSCharacterMovementComponent>(GetCharacterMovement()))
    {
        Movement->SetSprintHeld(false);
    }
}

void AMyALSCharacter::ResetControlInput()
{
    ClearMoveInput();
    StopJumpInput();
    UnCrouch();
    if (UMyALSCharacterMovementComponent* Movement = Cast<UMyALSCharacterMovementComponent>(GetCharacterMovement()))
    {
        Movement->ResetGaitSelection();
    }
}


