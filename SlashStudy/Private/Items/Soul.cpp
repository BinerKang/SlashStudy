// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetActorLocation().Z > DesiredZ)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, -DeltaTime * DriftSpeedPerSecond));
	}
}

void ASoul::BeginPlay()
{
	Super::BeginPlay();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this->GetOwner());
	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		GetActorLocation(),
		GetActorLocation() - FVector(0.f, 0.f, 2000.f),
		ObjectTypes,
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		HitResult,
		true
	);
	DesiredZ = HitResult.ImpactPoint.Z + DistanceToFloor;
}

void ASoul::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IPickupInterface* IPickup = Cast<IPickupInterface>(OtherActor))
	{
		IPickup->AddSouls(this->Souls);
		SpawnPickupEffect();
		SpawnPickupSound();
		Destroy();
	}
}
