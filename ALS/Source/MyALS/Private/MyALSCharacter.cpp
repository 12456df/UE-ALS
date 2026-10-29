#include "MyALSCharacter.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
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

void AMyALSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EnhancedInput)
    {
        return;
    }

    if (MoveAction)
    {
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyALSCharacter::HandleMove);
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Completed, this, &AMyALSCharacter::HandleMoveCompleted);
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Canceled, this, &AMyALSCharacter::HandleMoveCompleted);
    }
    if (LookAction)
    {
        EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyALSCharacter::HandleLook);
    }
    if (JumpAction)
    {
        EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyALSCharacter::HandleJumpStarted);
        EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyALSCharacter::HandleJumpCompleted);
    }
    if (CrouchAction)
    {
        EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMyALSCharacter::HandleCrouchStarted);
        EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AMyALSCharacter::HandleCrouchCompleted);
    }
    if (WalkAction)
    {
        EnhancedInput->BindAction(WalkAction, ETriggerEvent::Started, this, &AMyALSCharacter::HandleWalkStarted);
    }
    if (SprintAction)
    {
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &AMyALSCharacter::HandleSprintStarted);
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMyALSCharacter::HandleSprintCompleted);
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Canceled, this, &AMyALSCharacter::HandleSprintCompleted);
    }
}

void AMyALSCharacter::HandleMove(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
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

void AMyALSCharacter::HandleMoveCompleted()
{
    if (UMyALSCharacterMovementComponent* Movement = Cast<UMyALSCharacterMovementComponent>(GetCharacterMovement()))
    {
        Movement->SetMoveInput(FVector2D::ZeroVector);
    }
}

void AMyALSCharacter::HandleLook(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    AddControllerYawInput(Axis.X);
    AddControllerPitchInput(Axis.Y);
}

void AMyALSCharacter::HandleJumpStarted()
{
    Jump();
}

void AMyALSCharacter::HandleJumpCompleted()
{
    StopJumping();
}

void AMyALSCharacter::HandleCrouchStarted()
{
    Crouch();
}

void AMyALSCharacter::HandleCrouchCompleted()
{
    UnCrouch();
}

void AMyALSCharacter::HandleWalkStarted()
{
    if (UMyALSCharacterMovementComponent* Movement = Cast<UMyALSCharacterMovementComponent>(GetCharacterMovement()))
    {
        Movement->ToggleWalk();
    }
}

void AMyALSCharacter::HandleSprintStarted()
{
    if (UMyALSCharacterMovementComponent* Movement = Cast<UMyALSCharacterMovementComponent>(GetCharacterMovement()))
    {
        Movement->SetSprintHeld(true);
    }
}

void AMyALSCharacter::HandleSprintCompleted()
{
    if (UMyALSCharacterMovementComponent* Movement = Cast<UMyALSCharacterMovementComponent>(GetCharacterMovement()))
    {
        Movement->SetSprintHeld(false);
    }
}
