// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "SlashOverlay.generated.h"

class UProgressBar;
class UTextBlock;

/**
 * 
 */
UCLASS()
class SLASHSTUDY_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGoldCount(int32 Value);
	void SetSoulsCount(int32 Value);

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldCountText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SoulsCountText;
	
};
