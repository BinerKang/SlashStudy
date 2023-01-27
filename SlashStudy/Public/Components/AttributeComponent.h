// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASHSTUDY_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ReceiveDamage(float Damage);
	void UseStamina(float Cost);
	void UseDodgeStamina();
	float GetHealthPercent();
	float GetStaminaPercent();
	bool IsAlive();
	void AddGold(int32 Value);
	void AddSouls(int32 Value);
	bool HasEnoughStamina(float NeedStamina);
	bool HasEnoughDodgeStamina();
	void RegenStamina(float DeltaTime);

	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }

protected:
	virtual void BeginPlay() override;

private:	
	// Current Health
	UPROPERTY(EditAnywhere, Category = "Custom")
	float Health = 100.f;
		
	UPROPERTY(EditAnywhere, Category = "Custom")
	float MaxHealth = 100.f;

	// Current Stamina
	UPROPERTY(EditAnywhere, Category = "Custom")
	float Stamina = 120.f;

	UPROPERTY(EditAnywhere, Category = "Custom")
	float MaxStamina = 120.f;

	UPROPERTY(EditAnywhere, Category = "Custom")
	int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Custom")
	int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Custom")
	float DodgeCost = 15.f;

	UPROPERTY(EditAnywhere, Category = "Custom")
	float StaminaRegenPerSecond = 2.f;
};
