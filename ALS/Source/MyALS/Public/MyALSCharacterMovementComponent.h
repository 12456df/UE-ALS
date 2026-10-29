#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "MyALSCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum class Gait : uint8
{
    Walk,
    Jog,
    Sprint,
    Crouch
};

/** 步态速度由 CMC 统一决定；动画蓝图只读取结果，不反向设置运动速度。 */
UCLASS()
class MYALS_API UMyALSCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    virtual float GetMaxSpeed() const override;

    UFUNCTION(BlueprintPure, Category="MyALS|Locomotion")
    float GetGaitSpeed(Gait InGait) const;

    void ToggleWalk();
    void SetSprintHeld(bool bHeld);
    void ResetGaitSelection();
    void SetMoveInput(const FVector2D& Input);

    // 地面目标步态；空中仍由引擎的空中速度规则处理。
    UFUNCTION(BlueprintPure, Category="MyALS|Locomotion")
    Gait GetCurrentGait() const;

protected:
    virtual void OnRegister() override;

#if WITH_EDITOR
    virtual bool CanEditChange(const FProperty* InProperty) const override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    // 起始调参值；以实际动画位移、脚滑和试玩结果校准。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Locomotion|Speed", meta=(ClampMin="0.0"))
    float WalkSpeed = 300.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Locomotion|Speed", meta=(ClampMin="0.0"))
    float JogSpeed = 600.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Locomotion|Speed", meta=(ClampMin="0.0"))
    float SprintSpeed = 900.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Locomotion|Speed", meta=(ClampMin="0.0"))
    float CrouchSpeed = 300.0f;

private:
    void SyncEngineSpeedFields();

    bool bWalkSelected = false;
    bool bSprintHeld = false;
    FVector2D MoveInput = FVector2D::ZeroVector;
};
