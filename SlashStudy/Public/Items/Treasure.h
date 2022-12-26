// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class SLASHSTUDY_API ATreasure : public AItem
{
	GENERATED_BODY()
	
public:
	ATreasure();

protected:
	virtual void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Custom|Sounds")
	TObjectPtr<USoundBase> PickupSound;

	UPROPERTY(EditAnywhere, Category = "Custom")
	int32 Gold;
};
