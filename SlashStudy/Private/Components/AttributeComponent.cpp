// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

void UAttributeComponent::UseStamina(float Cost)
{
	Stamina = FMath::Clamp(Stamina - Cost, 0.f, MaxStamina);
}

void UAttributeComponent::UseDodgeStamina()
{
	UseStamina(DodgeCost);
}

float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

float UAttributeComponent::GetStaminaPercent()
{
	return Stamina / MaxStamina;
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

void UAttributeComponent::AddGold(int32 Value)
{
	Gold += Value;
}

void UAttributeComponent::AddSouls(int32 Value)
{
	Souls += Value;
}

bool UAttributeComponent::HasEnoughStamina(float NeedStamina)
{
	return Stamina >= NeedStamina;
}

bool UAttributeComponent::HasEnoughDodgeStamina()
{
	return HasEnoughStamina(DodgeCost);
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + DeltaTime * StaminaRegenPerSecond, 0.f, MaxStamina);
}

