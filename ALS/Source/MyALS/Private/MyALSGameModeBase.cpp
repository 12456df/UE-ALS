#include "MyALSGameModeBase.h"

#include "MyALSCharacter.h"
#include "MyALSPlayerController.h"

AMyALSGameModeBase::AMyALSGameModeBase()
{
    DefaultPawnClass = AMyALSCharacter::StaticClass();
    PlayerControllerClass = AMyALSPlayerController::StaticClass();
}
