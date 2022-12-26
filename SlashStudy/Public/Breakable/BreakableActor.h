// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class UCapsuleComponent;

UCLASS()
class SLASHSTUDY_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void GetHit(const FVector& ImpactPoint) override;

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldsLocation);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> Capsule;

	UFUNCTION()
	void OnBreakEvent(const FChaosBreakEvent& BreakEvent);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGeometryCollectionComponent> GeometryCollection;

	UPROPERTY(EditAnywhere, Category = "Custom|Sounds")
	TObjectPtr<USoundBase> BreakSound;

	UPROPERTY(EditAnywhere, Category = "Custom|Spawn")
	TArray<TSubclassOf<class ATreasure>> TreasureClasses;

	UPROPERTY(EditAnywhere, Category = "Custom|Spawn")
	FVector SpawnOffset;

	bool bBroken = false;
};
