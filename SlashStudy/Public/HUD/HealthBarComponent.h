// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

class UHealthBarWidget;

/**
 * 
 */
UCLASS()
class SLASHSTUDY_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UHealthBarComponent();

	/*
		Value Range In 0~1
	*/
	void SetHealthPercent(float Value); 

protected:
	UPROPERTY()
	TObjectPtr<UHealthBarWidget> HealthBarWidget;
	
};
