// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "kismet/GameplayStatics.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Geometry Collection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	// Make sure camera and pawns not move by breakable parts
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	// Enable Break Notify
	GeometryCollection->SetNotifyBreaks(true);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::OnBreakEvent);
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABreakableActor::GetHit(const FVector& ImpactPoint)
{
	if (bBroken) return;
	bBroken = true;

	if (BreakSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BreakSound, ImpactPoint);
	}
	CreateFields(ImpactPoint);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	if (TreasureClasses.Num() > 0 && GetWorld())
	{
		GetWorld()->SpawnActor<ATreasure>(
			TreasureClasses[FMath::RandRange(0, TreasureClasses.Num() - 1)],
			GetActorLocation() + SpawnOffset, 
			GetActorRotation()
			);
	}
}

void ABreakableActor::OnBreakEvent(const FChaosBreakEvent& BreakEvent)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString("Break..."));
	}
	this->SetLifeSpan(3.f);
}





