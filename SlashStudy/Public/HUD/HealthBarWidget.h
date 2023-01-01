// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class SLASHSTUDY_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget)) // Bind WBP HealthBar make sure both name same. 
	TObjectPtr<UProgressBar> HealthBar;
	
	void SetHealthBarPercent(float Value);
};
