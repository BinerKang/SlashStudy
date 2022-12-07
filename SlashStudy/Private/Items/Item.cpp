// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "SlashStudy/DebugMacros.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	// replace  DefaultSceneRoot
	SetRootComponent(ItemMesh);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
	{
		// %s need *Fstring
		FString Message = FString::Printf(TEXT("Item Name: %s"), *GetName());
		GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Cyan, Message);
		UE_LOG(LogTemp, Warning, TEXT(", Item Name: %s"), *GetName())
	}

	// SetActorLocation(FVector(0.f, 0.f, 300.f));
	// SetActorRotation(FRotator(0.f, 45.f, 0.f));

	FVector Location = GetActorLocation();
	FVector Forward = GetActorForwardVector();
	FVector EndLoc = Location + Forward * 100.f;
	DRAW_SPHERE(Location);
	// DRAW_LINE(Location, EndLoc);
	// DRAW_POINT(EndLoc);
	DRAW_VECTOR(Location, EndLoc);
	
}

float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// rate in unit : cm/s
	/*float MovementRate = 50.f;
	float RotationRate = 45.f;*/

	// rate * DeltaTime = (cm/s * s/frame) = cm/frame
	/*AddActorWorldOffset(FVector(MovementRate * DeltaTime, 0.f, 0.f));
	AddActorWorldRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));*/

	RunningTime += DeltaTime;
	// float DeltaZ = Amplitude * FMath::Sin(RunningTime * TimeConstant);
	// AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));

	//DRAW_SPHERE_SingleFrame(GetActorLocation());
	//DRAW_VECTOR_SingleFrame(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f);

	//FVector AvgVec = Avg(GetActorLocation(), FVector::ZeroVector);
	//DRAW_POINT_SingleFrame(AvgVec);
}

