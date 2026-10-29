#pragma once

#include "GameFramework/Character.h"
#include "MyALSCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class MYALS_API AMyALSCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    explicit AMyALSCharacter(const FObjectInitializer& ObjectInitializer);

    // Controller 解释输入；Character 只执行当前角色的操作意图。
    void ApplyMoveInput(const FVector2D& Axis);
    void ClearMoveInput();
    void ApplyLookInput(const FVector2D& Axis);
    void StartJumpInput();
    void StopJumpInput();
    void ToggleCrouchInput();
    void ToggleWalkInput();
    void StartSprintInput();
    void StopSprintInput();
    void ResetControlInput();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MyALS|Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MyALS|Camera")
    TObjectPtr<UCameraComponent> FollowCamera;

};
