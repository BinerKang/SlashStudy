// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class UBoxComponent;

/**
 * 
 */
UCLASS()
class SLASHSTUDY_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);
	
	// Used by One hit per swing.
	TArray<TObjectPtr<AActor>> IgnoreActors;
	/**
	* Getter and Setter
	*/
	FORCEINLINE TObjectPtr<UBoxComponent> GetWeaponBox() const { return WeaponBox; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool IsSameActorType(AActor* OtherActor);

	void BoxTrace(FHitResult& BoxHit);


private:
	UPROPERTY(EditAnywhere, Category = "Custom|Sounds")
	TObjectPtr<USoundBase> EquipSound;

	UPROPERTY(EditAnywhere, Category = "Custom")
	float Damage = 20.f;

	UPROPERTY(EditAnywhere, Category = "Custom")
	FVector BoxTraceExtend = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Custom")
	bool bShowBoxTraceDebug = false;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> WeaponBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceEnd;

};
