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
	float GetHealthPercent();
	bool IsAlive();

protected:
	virtual void BeginPlay() override;

private:	
	// Current Health
	UPROPERTY(EditAnywhere, Category = "Custom")
	float Health = 100.f;
		
	UPROPERTY(EditAnywhere, Category = "Custom")
	float MaxHealth = 100.f;
};
