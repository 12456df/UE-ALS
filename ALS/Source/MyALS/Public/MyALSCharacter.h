#pragma once

#include "GameFramework/Character.h"
#include "MyALSCharacter.generated.h"

class UCameraComponent;
class UInputAction;
class USpringArmComponent;
struct FInputActionValue;

UCLASS()
class MYALS_API AMyALSCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    explicit AMyALSCharacter(const FObjectInitializer& ObjectInitializer);

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MyALS|Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MyALS|Camera")
    TObjectPtr<UCameraComponent> FollowCamera;

    // 在蓝图子类中指定 IA 资产；移动和视角均要求 Axis2D。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input")
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input")
    TObjectPtr<UInputAction> CrouchAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input")
    TObjectPtr<UInputAction> WalkAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input")
    TObjectPtr<UInputAction> SprintAction;

private:
    void HandleMove(const FInputActionValue& Value);
    void HandleMoveCompleted();
    void HandleLook(const FInputActionValue& Value);
    void HandleJumpStarted();
    void HandleJumpCompleted();
    void HandleCrouchStarted();
    void HandleCrouchCompleted();
    void HandleWalkStarted();
    void HandleSprintStarted();
    void HandleSprintCompleted();
};
