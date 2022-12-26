// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;

enum class EItemState : uint8 
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class SLASHSTUDY_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// BlueprintReadWrite: Expose to Event Graph and can not private variable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Sine Parameters") // EditAnywhere EditDefaultsOnly EditInstanceOnly
	float TimeConstant = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Sine Parameters")
	float Amplitude = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
	float RotationRate = 0.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	UFUNCTION(BlueprintPure) // BlueprintPure:pure function  BlueprintCallable: function
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();

	template<typename T>
	T Avg(T First, T Second);

	EItemState ItemState = EItemState::EIS_Hovering;

	UFUNCTION()
	virtual void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

private:

	// BlueprintReadOnly: Expose to Event Graph,use this parameter meta = (AllowPrivateAccess = "true") for paivate var
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) //VisibleAnywhere VisibleDefaultsOnly VisibleInstanceOnly
	float RunningTime;

};

template<typename T>
inline T AItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}
