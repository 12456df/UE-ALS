#pragma once

#include "Engine/DataAsset.h"
#include "MyALSInputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

/** 一个控制方案的资产引用；按键映射在 IMC，动作语义在 IA。 */
UCLASS(BlueprintType)
class MYALS_API UMyALSInputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input|Context")
    TObjectPtr<UInputMappingContext> GroundMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input|Context")
    TObjectPtr<UInputMappingContext> SwimmingMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input|Context")
    TObjectPtr<UInputMappingContext> VehicleMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input|Action")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input|Action")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input|Action")
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input|Action")
    TObjectPtr<UInputAction> CrouchAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input|Action")
    TObjectPtr<UInputAction> WalkAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input|Action")
    TObjectPtr<UInputAction> SprintAction;
};
