#include "MyALSPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"

void AMyALSPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController() && DefaultMappingContext)
    {
        if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
            {
                InputSubsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
}
