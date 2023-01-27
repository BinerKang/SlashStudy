// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"

void ASlashHUD::BeginPlay()
{
	Super::BeginPlay();
	if (SlashOverlayClass)
	{
		if (UWorld* World = GetWorld())
		{
			if (APlayerController* Controller = World->GetFirstPlayerController())
			{
				SlashOverlay = CreateWidget<USlashOverlay>(Controller, SlashOverlayClass);
				SlashOverlay->AddToViewport();
			}
		}
	}
}
