// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "SlashStudy/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Characters/SlashCharacter.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	// replace  DefaultSceneRoot
	SetRootComponent(ItemMesh);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	Sphere->SetupAttachment(GetRootComponent());
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	// Bind Callback Function to OnComponentBeginOverlap OnComponentEndOverlap 
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlapBegin);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereOverlapEnd);
	//if (GEngine)
	//{
	//	// %s need *Fstring
	//	FString Message = FString::Printf(TEXT("Item Name: %s"), *GetName());
	//	GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Cyan, Message);
	//	UE_LOG(LogTemp, Warning, TEXT(", Item Name: %s"), *GetName())
	//}

	// SetActorLocation(FVector(0.f, 0.f, 300.f));
	// SetActorRotation(FRotator(0.f, 45.f, 0.f));

	//FVector Location = GetActorLocation();
	//FVector Forward = GetActorForwardVector();
	//FVector EndLoc = Location + Forward * 100.f;
	//DRAW_SPHERE(Location);
	// DRAW_LINE(Location, EndLoc);
	// DRAW_POINT(EndLoc);
	//DRAW_VECTOR(Location, EndLoc);
	
}

float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

void AItem::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASlashCharacter* SlashChr = Cast<ASlashCharacter>(OtherActor))
	{
		SlashChr->SetOverlappingItem(this);
	}
}

void AItem::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ASlashCharacter* SlashChr = Cast<ASlashCharacter>(OtherActor))
	{
		SlashChr->SetOverlappingItem(nullptr);
	}
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
	*/

	if (ItemState == EItemState::EIS_Hovering && WasRecentlyRendered(0.1))
	{
		RunningTime += DeltaTime;
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));

		if (RotationRate != 0)
		{
			AddActorWorldRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));
		}
	}

	//DRAW_SPHERE_SingleFrame(GetActorLocation());
	//DRAW_VECTOR_SingleFrame(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f);

	//FVector AvgVec = Avg(GetActorLocation(), FVector::ZeroVector);
	//DRAW_POINT_SingleFrame(AvgVec);
}

