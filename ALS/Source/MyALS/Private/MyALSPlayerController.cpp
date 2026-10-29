#include "MyALSPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "MyALSCharacter.h"
#include "MyALSInputConfig.h"

void AMyALSPlayerController::BeginPlay()
{
    Super::BeginPlay();
    if (IsLocalController())
    {
        SetControlMode(ControlMode::Ground);
    }
}

void AMyALSPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (!InputConfig)
    {
        UE_LOG(LogTemp, Error, TEXT("MyALS: %s 没有指定 InputConfig，输入不会绑定。"), *GetName());
        return;
    }

    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
    
    if (!EnhancedInput)
    {
        UE_LOG(LogTemp, Error, TEXT("MyALS: %s 的 InputComponent 不是 EnhancedInputComponent。"), *GetName());
        return;
    }

    if (!InputConfig->MoveAction || !InputConfig->LookAction || !InputConfig->JumpAction ||
        !InputConfig->CrouchAction || !InputConfig->WalkAction || !InputConfig->SprintAction)
    {
        UE_LOG(LogTemp, Error, TEXT("MyALS: InputConfig %s 缺少地面角色所需的 IA。"), *GetNameSafe(InputConfig));
        return;
    }

    EnhancedInput->BindAction(InputConfig->MoveAction, ETriggerEvent::Triggered, this, &AMyALSPlayerController::HandleMove);
    EnhancedInput->BindAction(InputConfig->MoveAction, ETriggerEvent::Completed, this, &AMyALSPlayerController::HandleMoveCompleted);
    EnhancedInput->BindAction(InputConfig->MoveAction, ETriggerEvent::Canceled, this, &AMyALSPlayerController::HandleMoveCompleted);
    EnhancedInput->BindAction(InputConfig->LookAction, ETriggerEvent::Triggered, this, &AMyALSPlayerController::HandleLook);
    EnhancedInput->BindAction(InputConfig->JumpAction, ETriggerEvent::Started, this, &AMyALSPlayerController::HandleJumpStarted);
    EnhancedInput->BindAction(InputConfig->JumpAction, ETriggerEvent::Completed, this, &AMyALSPlayerController::HandleJumpCompleted);
    EnhancedInput->BindAction(InputConfig->CrouchAction, ETriggerEvent::Started, this, &AMyALSPlayerController::HandleCrouchStarted);
    EnhancedInput->BindAction(InputConfig->WalkAction, ETriggerEvent::Started, this, &AMyALSPlayerController::HandleWalkStarted);
    EnhancedInput->BindAction(InputConfig->SprintAction, ETriggerEvent::Started, this, &AMyALSPlayerController::HandleSprintStarted);
    EnhancedInput->BindAction(InputConfig->SprintAction, ETriggerEvent::Completed, this, &AMyALSPlayerController::HandleSprintCompleted);
    EnhancedInput->BindAction(InputConfig->SprintAction, ETriggerEvent::Canceled, this, &AMyALSPlayerController::HandleSprintCompleted);
}

void AMyALSPlayerController::OnUnPossess()
{
    if (AMyALSCharacter* ALSCharacter = GetControlledCharacter())
    {
        ALSCharacter->ResetControlInput();
    }
    Super::OnUnPossess();
}

const UInputMappingContext* AMyALSPlayerController::GetMappingContext(ControlMode Mode) const
{
    if (!InputConfig)
    {
        return nullptr;
    }

    switch (Mode)
    {
    case ControlMode::Ground: return InputConfig->GroundMappingContext;
    case ControlMode::Swimming: return InputConfig->SwimmingMappingContext;
    case ControlMode::Vehicle: return InputConfig->VehicleMappingContext;
    default: return nullptr;
    }
}

bool AMyALSPlayerController::SetControlMode(ControlMode NewMode)
{
    if (!IsLocalController())
    {
        return false;
    }

    const UInputMappingContext* NewContext = GetMappingContext(NewMode);
    if (!NewContext)
    {
        UE_LOG(LogTemp, Warning, TEXT("MyALS: 控制模式 %d 没有配置 IMC，保留当前模式。"), static_cast<int32>(NewMode));
        return false;
    }

    ULocalPlayer* LocalPlayer = GetLocalPlayer();
    UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer
        ? ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer) : nullptr;
    if (!InputSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("MyALS: 无法取得 Enhanced Input Local Player Subsystem。"));
        return false;
    }

    if (bMappingContextApplied && CurrentControlMode == NewMode)
    {
        return true;
    }

    if (AMyALSCharacter* ALSCharacter = GetControlledCharacter())
    {
        ALSCharacter->ResetControlInput();
    }

    if (bMappingContextApplied)
    {
        InputSubsystem->RemoveMappingContext(GetMappingContext(CurrentControlMode));
    }
    InputSubsystem->AddMappingContext(NewContext, 0);
    CurrentControlMode = NewMode;
    bMappingContextApplied = true;
    return true;
}

AMyALSCharacter* AMyALSPlayerController::GetControlledCharacter() const
{
    return Cast<AMyALSCharacter>(GetPawn());
}

void AMyALSPlayerController::HandleMove(const FInputActionValue& Value)
{
    if (CurrentControlMode != ControlMode::Ground) return;
    if (AMyALSCharacter* ALSCharacter = GetControlledCharacter()) ALSCharacter->ApplyMoveInput(Value.Get<FVector2D>());
}

void AMyALSPlayerController::HandleMoveCompleted()
{
    if (AMyALSCharacter* ALSCharacter = GetControlledCharacter()) ALSCharacter->ClearMoveInput();
}

void AMyALSPlayerController::HandleLook(const FInputActionValue& Value)
{
    if (CurrentControlMode != ControlMode::Ground) return;
    if (AMyALSCharacter* ALSCharacter = GetControlledCharacter()) ALSCharacter->ApplyLookInput(Value.Get<FVector2D>());
}

void AMyALSPlayerController::HandleJumpStarted()
{
    if (CurrentControlMode != ControlMode::Ground) return;
    if (AMyALSCharacter* ALSCharacter = GetControlledCharacter()) ALSCharacter->StartJumpInput();
}

void AMyALSPlayerController::HandleJumpCompleted()
{
    if (AMyALSCharacter* ALSCharacter = GetControlledCharacter()) ALSCharacter->StopJumpInput();
}

void AMyALSPlayerController::HandleCrouchStarted()
{
    if (CurrentControlMode != ControlMode::Ground) return;
    if (AMyALSCharacter* ALSCharacter = GetControlledCharacter()) ALSCharacter->ToggleCrouchInput();
}

void AMyALSPlayerController::HandleWalkStarted()
{
    if (CurrentControlMode != ControlMode::Ground) return;
    if (AMyALSCharacter* ALSCharacter = GetControlledCharacter()) ALSCharacter->ToggleWalkInput();
}

void AMyALSPlayerController::HandleSprintStarted()
{
    if (CurrentControlMode != ControlMode::Ground) return;
    if (AMyALSCharacter* ALSCharacter = GetControlledCharacter()) ALSCharacter->StartSprintInput();
}

void AMyALSPlayerController::HandleSprintCompleted()
{
    if (AMyALSCharacter* ALSCharacter = GetControlledCharacter()) ALSCharacter->StopSprintInput();
}
