// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBarWidget.h"

UHealthBarComponent::UHealthBarComponent()
{
}

void UHealthBarComponent::SetHealthPercent(float Value)
{
	if (!HealthBarWidget)
	{
		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Orange, FString::Printf(TEXT("Health Percent: %f"), Value));
		}*/
		// Cast is expensive, Use Variable to store.
		HealthBarWidget = Cast<UHealthBarWidget>(GetUserWidgetObject());
		if (HealthBarWidget)
		{
			HealthBarWidget->SetHealthBarPercent(Value);
		}
	}
	else
	{
		HealthBarWidget->SetHealthBarPercent(Value);
	}
}
