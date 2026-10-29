#include "MyALSCharacterMovementComponent.h"

#if WITH_EDITOR
#include "UObject/UnrealType.h"
#endif

void UMyALSCharacterMovementComponent::OnRegister()
{
    SyncEngineSpeedFields();
    Super::OnRegister();
}

void UMyALSCharacterMovementComponent::SyncEngineSpeedFields()
{
    // 引擎的其他运动路径仍直接读取这两个字段，保持它们与自定义配置一致。
    MaxWalkSpeed = JogSpeed;
    MaxWalkSpeedCrouched = CrouchSpeed;
}

#if WITH_EDITOR
bool UMyALSCharacterMovementComponent::CanEditChange(const FProperty* InProperty) const
{
    if (InProperty &&
        (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UCharacterMovementComponent, MaxWalkSpeed) ||
         InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UCharacterMovementComponent, MaxWalkSpeedCrouched)))
    {
        return false;
    }

    return Super::CanEditChange(InProperty);
}

void UMyALSCharacterMovementComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    SyncEngineSpeedFields();
}
#endif

float UMyALSCharacterMovementComponent::GetGaitSpeed(Gait InGait) const
{
    switch (InGait)
    {
    case Gait::Walk:
        return WalkSpeed;
    case Gait::Sprint:
        return SprintSpeed;
    case Gait::Crouch:
        return CrouchSpeed;
    case Gait::Jog:
    default:
        return JogSpeed;
    }
}

void UMyALSCharacterMovementComponent::ToggleWalk()
{
    bWalkSelected = !bWalkSelected;
}

void UMyALSCharacterMovementComponent::SetSprintHeld(bool bHeld)
{
    bSprintHeld = bHeld;
}

void UMyALSCharacterMovementComponent::ResetGaitSelection()
{
    bWalkSelected = false;
    bSprintHeld = false;
}

void UMyALSCharacterMovementComponent::SetMoveInput(const FVector2D& Input)
{
    MoveInput = Input;
}

Gait UMyALSCharacterMovementComponent::GetCurrentGait() const
{
    if (IsCrouching())
    {
        return Gait::Crouch;
    }

    // 前进及前斜向可冲刺；纯侧移、后退、空中和蹲伏均不可冲刺。
    if (bSprintHeld && IsMovingOnGround() && MoveInput.Y > 0.1f)
    {
        return Gait::Sprint;
    }

    return bWalkSelected ? Gait::Walk : Gait::Jog;
}

float UMyALSCharacterMovementComponent::GetMaxSpeed() const
{
    // 空中和其他移动模式沿用引擎规则；继承字段已与自定义速度同步。
    if (MovementMode != MOVE_Walking && MovementMode != MOVE_NavWalking)
    {
        return Super::GetMaxSpeed();
    }

    return GetGaitSpeed(GetCurrentGait());
}
