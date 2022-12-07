// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashAnimInstance.h"
#include "Characters/SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	SlashChr = Cast<ASlashCharacter>(TryGetPawnOwner());
	if (SlashChr)
	{
		SlashChrMoveComp = SlashChr->GetCharacterMovement();
	}
}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (SlashChrMoveComp)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(SlashChrMoveComp->Velocity);
		IsFalling = SlashChrMoveComp->IsFalling();
	}
}
