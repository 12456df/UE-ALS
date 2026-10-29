#pragma once

#include "GameFramework/PlayerController.h"
#include "MyALSPlayerController.generated.h"

class UInputMappingContext;
class UMyALSInputConfig;
class AMyALSCharacter;
struct FInputActionValue;

UENUM(BlueprintType)
enum class ControlMode : uint8
{
    Ground,
    Swimming,
    Vehicle
};

UCLASS()
class MYALS_API AMyALSPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void OnUnPossess() override;

    // IA 与 IMC 只从这一份配置读取。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input")
    TObjectPtr<UMyALSInputConfig> InputConfig;

public:
    UFUNCTION(BlueprintCallable, Category="MyALS|Input")
    bool SetControlMode(ControlMode NewMode);

    UFUNCTION(BlueprintPure, Category="MyALS|Input")
    ControlMode GetControlMode() const { return CurrentControlMode; }

private:
    const UInputMappingContext* GetMappingContext(ControlMode Mode) const;
    AMyALSCharacter* GetControlledCharacter() const;
    void HandleMove(const FInputActionValue& Value);
    void HandleMoveCompleted();
    void HandleLook(const FInputActionValue& Value);
    void HandleJumpStarted();
    void HandleJumpCompleted();
    void HandleCrouchStarted();
    void HandleWalkStarted();
    void HandleSprintStarted();
    void HandleSprintCompleted();

    ControlMode CurrentControlMode = ControlMode::Ground;
    bool bMappingContextApplied = false;
};
