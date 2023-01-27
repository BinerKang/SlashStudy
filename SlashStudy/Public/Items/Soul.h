// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

/**
 * 
 */
UCLASS()
class SLASHSTUDY_API ASoul : public AItem
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetSouls(int32 Value) { Souls = Value; }

protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Custom")
	int32 Souls = 1;

	UPROPERTY(EditAnywhere, Category = "Custom")
	float DistanceToFloor = 100.f;

	UPROPERTY(EditAnywhere, Category = "Custom")
	float DriftSpeedPerSecond = 20.f;

	float DesiredZ;
};
