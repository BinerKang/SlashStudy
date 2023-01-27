// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Interfaces/PickupInterface.h"

ATreasure::ATreasure()
{
}

void ATreasure::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IPickupInterface* IPickup = Cast<IPickupInterface>(OtherActor))
	{
		IPickup->AddGold(this->Gold);
		SpawnPickupSound();
		Destroy();
	}
}
