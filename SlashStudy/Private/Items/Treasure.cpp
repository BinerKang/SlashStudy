// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "kismet/GameplayStatics.h"

ATreasure::ATreasure()
{
}

void ATreasure::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
	Destroy();
}
