#pragma once

#include "Animation/AnimInstance.h"
#include "MyALSCharacterMovementComponent.h"
#include "MyALSAnimInstance.generated.h"

class AMyALSCharacter;

UCLASS()
class MYALS_API UMyALSAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    UPROPERTY(Transient, BlueprintReadOnly, Category="MyALS|Locomotion")
    FVector Velocity = FVector::ZeroVector;

    UPROPERTY(Transient, BlueprintReadOnly, Category="MyALS|Locomotion")
    FVector Acceleration = FVector::ZeroVector;

    UPROPERTY(Transient, BlueprintReadOnly, Category="MyALS|Locomotion")
    float Direction = 0.0f;

    UPROPERTY(Transient, BlueprintReadOnly, Category="MyALS|Locomotion")
    float GroundSpeed = 0.0f;

    UPROPERTY(Transient, BlueprintReadOnly, Category="MyALS|Locomotion")
    bool bIsFalling = false;

    UPROPERTY(Transient, BlueprintReadOnly, Category="MyALS|Locomotion")
    bool bIsCrouching = false;

    UPROPERTY(Transient, BlueprintReadOnly, Category="MyALS|Locomotion")
    bool bIsSprinting = false;

    // 地面目标步态；InAir 状态应优先使用 bIsFalling。
    UPROPERTY(Transient, BlueprintReadOnly, Category="MyALS|Locomotion")
    Gait CurrentGait = Gait::Jog;

private:
    UPROPERTY(Transient)
    TObjectPtr<AMyALSCharacter> Character;

    UPROPERTY(Transient)
    TObjectPtr<UMyALSCharacterMovementComponent> MovementComponent;
};
