#include "MyALSAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"
#include "MyALSCharacter.h"

void UMyALSAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    Character = Cast<AMyALSCharacter>(TryGetPawnOwner());
    MovementComponent = Character ? Cast<UMyALSCharacterMovementComponent>(Character->GetCharacterMovement()) : nullptr;
}

void UMyALSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!IsValid(Character) || !IsValid(MovementComponent))
    {
        Character = Cast<AMyALSCharacter>(TryGetPawnOwner());
        MovementComponent = Character ? Cast<UMyALSCharacterMovementComponent>(Character->GetCharacterMovement()) : nullptr;
    }

    if (!Character || !MovementComponent)
    {
        Velocity = FVector::ZeroVector;
        Acceleration = FVector::ZeroVector;
        Direction = 0.0f;
        GroundSpeed = 0.0f;
        bIsFalling = false;
        bIsCrouching = false;
        bIsSprinting = false;
        CurrentGait = Gait::Jog;
        return;
    }

    Velocity = Character->GetVelocity();
    Acceleration = MovementComponent->GetCurrentAcceleration();
    Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
    GroundSpeed = Velocity.Size2D();
    bIsFalling = MovementComponent->IsFalling();
    bIsCrouching = MovementComponent->IsCrouching();
    CurrentGait = MovementComponent->GetCurrentGait();
    bIsSprinting = CurrentGait == Gait::Sprint && !bIsFalling;
}
