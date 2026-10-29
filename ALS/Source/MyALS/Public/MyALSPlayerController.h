#pragma once

#include "GameFramework/PlayerController.h"
#include "MyALSPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class MYALS_API AMyALSPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    // 蓝图子类指定 IMC，避免在 C++ 中硬编码资产路径。
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyALS|Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;
};
